#include <dma.h>
#include <dma_tags.h>

#include <gif_tags.h>
#include <gs_gp.h>

#include <packet.h>
#include <gs_psm.h>
#include <graph.h>
#include <draw.h>

// framebuffers
static int fbp[2] = {0,0};

static int vsync_callback = -1;
static int interlacing = 0;
static int context = 0;

// qwords
static qword_t *flip_q;
static qword_t *draw_q;
static qword_t *tex_q;

// packets
static packet_t *flip_packet;
static packet_t *tex_packet;
static packet_t *draw_packet;

// Register settings
static framebuffer_t frame;
static clutbuffer_t clut;
static texbuffer_t tex;

void video_packets_init(void)
{

	tex_packet  = packet_init(50,PACKET_UCAB);
	draw_packet = packet_init(100,PACKET_UCAB);
	flip_packet = packet_init(6,PACKET_UCAB);

}

void video_packets_free()
{

	packet_free(tex_packet);
	packet_free(draw_packet);
	packet_free(flip_packet);

}

void video_init_dmac(void)
{
	dma_channel_initialize(DMA_CHANNEL_GIF,NULL,0);
	dma_channel_fast_waits(DMA_CHANNEL_GIF);
}

void video_init_framebuffer(int width, int height)
{

	graph_vram_clear();

	// fbp[0] always equals 0
	fbp[0] = graph_vram_allocate(width,height,GS_PSM_16S,GRAPH_ALIGN_PAGE);
	fbp[1] = graph_vram_allocate(width,height,GS_PSM_16S,GRAPH_ALIGN_PAGE);

}

int video_vsync_handler(void)
{

	if (interlacing == GRAPH_MODE_NONINTERLACED)
	{
		graph_set_framebuffer(1,fbp[context],frame.width,GS_PSM_16S,0,0);
	}
	else
	{
		graph_set_framebuffer_filtered(fbp[context],frame.width,GS_PSM_16S,0,0);
	}

	asm("sync.l");
	asm("ei");

	return 0;

}

void video_init_screen(int x, int y, int width, int height, int interlace, int mode)
{

	context = 0;

	switch (mode)
	{
		case GRAPH_MODE_HDTV_1080I:
		case GRAPH_MODE_NTSC:
		case GRAPH_MODE_PAL:
		{
			if (interlace)
			{
				interlacing = GRAPH_MODE_INTERLACED;
				graph_set_mode(GRAPH_MODE_INTERLACED,mode,GRAPH_MODE_FIELD,GRAPH_ENABLE);
			}
			else
			{
				interlacing = GRAPH_MODE_NONINTERLACED;
				graph_set_mode(GRAPH_MODE_NONINTERLACED,mode,GRAPH_MODE_FRAME,GRAPH_DISABLE);
			}
			break;
		}
		default:
		{
			graph_set_mode(GRAPH_MODE_NONINTERLACED,mode,GRAPH_MODE_FRAME,GRAPH_DISABLE);
			break;
		}
	}

	graph_set_screen(x,y,width,height);
	graph_set_bgcolor(0,0,0);

	switch (mode)
	{
		case GRAPH_MODE_HDTV_1080I:
		case GRAPH_MODE_NTSC:
		case GRAPH_MODE_PAL:
		{
			if (interlacing)
			{
				graph_set_framebuffer(1,fbp[0],width,GS_PSM_16S,0,0);
			}
			else
			{
				graph_set_framebuffer_filtered(fbp[0],width,GS_PSM_16S,0,0);
			}
			break;
		}
		default:
		{
			graph_set_framebuffer(1,fbp[0],width,GS_PSM_16S,0,0);
			break;
		}
	}

	graph_enable_output();

}

void video_enable_vsync_handler()
{

	// Prevent adding multiple handlers
	if (vsync_callback != -1)
	{
		graph_remove_vsync_handler(vsync_callback);
	}

	vsync_callback = graph_add_vsync_handler(video_vsync_handler);

}

void video_disable_vsync_handler(void)
{

	// Prevent removing nonexistent handler
	if (vsync_callback == -1)
	{
		return;
	}

	graph_remove_vsync_handler(vsync_callback);

	vsync_callback = -1;

}

void video_init_draw_env(int width, int height)
{

	packet_t *packet = packet_init(20,PACKET_NORMAL);
	qword_t *q = packet->data;

	zbuffer_t z;

	frame.width = width;
	frame.height = height;
	frame.psm = GS_PSM_16S;
	frame.mask = 0;

	frame.address = fbp[0];

	z.enable = 0;
	z.method = ZTEST_METHOD_GREATER;
	z.address = 0;
	z.mask = 1;
	z.zsm = 0;

	q = draw_setup_environment(q,0,&frame,&z);
	q = draw_dithering(q,DRAW_ENABLE);

	PACK_GIFTAG(q, GS_SET_PRMODECONT(PRIM_OVERRIDE_ENABLE),GS_REG_PRMODECONT);
	q++;

	q = draw_finish(q);

	dma_channel_send_normal(DMA_CHANNEL_GIF,packet->data, q - packet->data, 0,0);
	dma_wait_fast();

	packet_free(packet);

}

void video_init_texbuffer(int width, int height, int tex_psm, int clut_psm)
{

	// Sets up the texture buffer attributes
	tex.width = width;
	tex.psm = tex_psm;

	tex.address = graph_vram_allocate(width,height,tex_psm,GRAPH_ALIGN_BLOCK);

	if (tex_psm == GS_PSM_8)
	{
		clut.address = graph_vram_allocate(16,16,clut_psm,GRAPH_ALIGN_BLOCK);
	}
	else if (tex_psm == GS_PSM_4)
	{
		clut.address = graph_vram_allocate(8,2,clut_psm,GRAPH_ALIGN_BLOCK);
	}
	else
	{

		clut.address = 0;
	}

}

void video_flip_buffer()
{

	frame.address = fbp[context];

	flip_q = flip_packet->data;

	flip_q = draw_framebuffer(flip_q,0,&frame);

	dma_wait_fast();
	dma_channel_send_normal_ucab(DMA_CHANNEL_GIF,flip_packet->data,flip_q - flip_packet->data, 0);
	dma_wait_fast();

}

qword_t *video_texture_flush(qword_t *q)
{
	// Flush texture buffer
	DMATAG_END(q,3,0,0,0);
	q++;
	PACK_GIFTAG(q,GIF_SET_TAG(2,1,0,0,GIF_FLG_PACKED,1),GIF_REG_AD);
	q++;
	PACK_GIFTAG(q,1,GS_REG_TEXFLUSH);
	q++;
	PACK_GIFTAG(q,1,GS_REG_FINISH);
	q++;

	return q;
}

void video_send_packet(int width, int height, void *texbuf,void *clutbuf)
{

	tex_q = tex_packet->data;

	// Sends the texture buffer whole, not just a texture
	tex_q = draw_texture_transfer(tex_q,texbuf,width,height,tex.psm,tex.address,tex.width);

	if (clutbuf != NULL)
	{
		if (tex.psm == GS_PSM_8)
		{
			tex_q = draw_texture_transfer(tex_q,clutbuf,16,16,clut.psm,clut.address,64);
		}
		else if (tex.psm == GS_PSM_4)
		{
			tex_q = draw_texture_transfer(tex_q,clutbuf,8,2,clut.psm,clut.address,64);
		}
	}

	tex_q = video_texture_flush(tex_q);

	tex_packet->qwc = tex_q - tex_packet->data;

}

void video_draw_packet(int width, int height, int tex_psm, int clut_psm)
{

	qword_t *dmatag;

	texrect_t rect;
	lod_t lod;

	// Setup an individual texture's information here
	tex.info.width = draw_log2(width);
	tex.info.height = draw_log2(height);
	tex.info.components = TEXTURE_COMPONENTS_RGB;
	tex.info.function = TEXTURE_FUNCTION_DECAL;

	clut.storage_mode = CLUT_STORAGE_MODE1;
	clut.start = 0;
	clut.psm = clut_psm;

	if (tex_psm == GS_PSM_8 || tex_psm == GS_PSM_4)
	{
		clut.load_method = CLUT_LOAD;
	}
	else
	{
		clut.load_method = CLUT_NO_LOAD;
	}

	rect.v0.x = 0.0f;
	rect.v0.y = 0.0f;
	rect.v0.z = 10;

	rect.v1.x = rect.v0.x + (width-1);
	rect.v1.y = rect.v0.y + (height-1);
	rect.v1.z = rect.v0.z;

	rect.t0.u = 0.5f;
	rect.t0.v = 0.5f;

	rect.t1.u = ((float)width)-1.5f;
	rect.t1.v = ((float)height)-1.5f;

	rect.color.r = 0x80;
	rect.color.g = 0x80;
	rect.color.b = 0x80;
	rect.color.a = 0x80;
	rect.color.q = 1.0f;

	lod.calculation = LOD_USE_K;
	lod.max_level = 0;
	lod.mag_filter = LOD_MAG_NEAREST;
	lod.min_filter = LOD_MIN_NEAREST;
	lod.l = 0;
	lod.k = 0;

	// Create the packet
	dmatag = draw_q = draw_packet->data;
	draw_q++;

	draw_q = draw_clear(draw_q,0,0,0,width,height,0x00,0x00,0x00);

	draw_q = draw_texture_sampling(draw_q,0,&lod);
	draw_q = draw_texturebuffer(draw_q,0,&tex,&clut);

	draw_q = draw_rect_textured_strips(draw_q,0,&rect);

	draw_q = draw_finish(draw_q);

	DMATAG_END(dmatag,draw_q-dmatag-1,0,0,0);

	draw_packet->qwc = draw_q - draw_packet->data;

}

void video_send_texture()
{

	dma_wait_fast();
	dma_channel_send_chain_ucab(DMA_CHANNEL_GIF,tex_packet->data, tex_q - tex_packet->data, 0);

}

void video_draw_texture()
{

	draw_wait_finish();

	//printf("size = %d\n",draw_packet->qwc);

	dma_wait_fast();
	dma_channel_send_chain_ucab(DMA_CHANNEL_GIF,draw_packet->data, draw_packet->qwc, 0);

	draw_wait_finish();

}

void video_sync_flip()
{

	if (graph_check_vsync())
	{

		graph_start_vsync();

		context ^= 1;

		// We need to flip buffers outside of the chain, for some reason.
		video_flip_buffer();

	}

}

void video_sync_wait()
{

	graph_wait_vsync();

	if (interlacing == GRAPH_MODE_NONINTERLACED)
	{
		graph_set_framebuffer(1,fbp[context],frame.width,GS_PSM_16S,0,0);
	}
	else
	{
		graph_set_framebuffer_filtered(fbp[context],frame.width,GS_PSM_16S,0,0);
	}

	context ^= 1;

	video_flip_buffer();

}
