#include "image.h"
#include "cc.h"
#include <stdlib.h>
#include <assert.h>

//////////////////////////////////////////////////////////////////////////
#include "libpng/png.h"

typedef struct 
{
	unsigned char* data;
	int size;
	int offset;
}tImageSource;

static void pngReadCallback(png_structp png_ptr, png_bytep data, png_size_t length)
{
	tImageSource* isource = (tImageSource*)png_get_io_ptr(png_ptr);

	if((int)(isource->offset + length) <= isource->size)
	{
		memcpy(data, isource->data+isource->offset, length);
		isource->offset += length;
	}
	else
	{
		png_error(png_ptr, "pngReaderCallback failed");
	}
}

#define CC_RGB_PREMULTIPLY_ALPHA(vr, vg, vb, va) \
	(unsigned)(((unsigned)((unsigned char)(vr) * ((unsigned char)(va) + 1)) >> 8) | \
	((unsigned)((unsigned char)(vg) * ((unsigned char)(va) + 1) >> 8) << 8) | \
	((unsigned)((unsigned char)(vb) * ((unsigned char)(va) + 1) >> 8) << 16) | \
	((unsigned)(unsigned char)(va) << 24))

bool create_image_png(image* img, void* pData,int nDatalen)
{
	// length of bytes to check if it is a valid png file
#define PNGSIGSIZE  8
	bool bRet = false;
	png_byte        header[PNGSIGSIZE]   = {0}; 
	png_structp     png_ptr     =   0;
	png_infop       info_ptr    = 0;

	do 
	{
		// png header len is 8 bytes
		CC_BREAK_IF(nDatalen < PNGSIGSIZE);

		// check the data is png or not
		memcpy(header, pData, PNGSIGSIZE);
		CC_BREAK_IF(png_sig_cmp(header, 0, PNGSIGSIZE));

		// init png_struct
		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
		CC_BREAK_IF(! png_ptr);

		// init png_info
		info_ptr = png_create_info_struct(png_ptr);
		CC_BREAK_IF(!info_ptr);

#if (CC_TARGET_PLATFORM != CC_PLATFORM_BADA)
		CC_BREAK_IF(setjmp(png_jmpbuf(png_ptr)));
#endif

		// set the read call back function
		tImageSource imageSource;
		imageSource.data    = (unsigned char*)pData;
		imageSource.size    = nDatalen;
		imageSource.offset  = 0;
		png_set_read_fn(png_ptr, &imageSource, pngReadCallback);

		// read png header info

		// read png file info
		png_read_info(png_ptr, info_ptr);

		img->m_width = png_get_image_width(png_ptr, info_ptr);
		img->m_height = png_get_image_height(png_ptr, info_ptr);
		img->m_bits_component = png_get_bit_depth(png_ptr, info_ptr);
		img->m_bits_pixel = png_get_channels(png_ptr, info_ptr);
		png_uint_32 color_type = png_get_color_type(png_ptr, info_ptr);

		//CCLOG("color type %u", color_type);

		// force palette images to be expanded to 24-bit RGB
		// it may include alpha channel
		if (color_type == PNG_COLOR_TYPE_PALETTE)
		{
			png_set_palette_to_rgb(png_ptr);
		}
		// low-bit-depth grayscale images are to be expanded to 8 bits
		if (color_type == PNG_COLOR_TYPE_GRAY && img->m_bits_component < 8)
		{
			png_set_expand_gray_1_2_4_to_8(png_ptr);
		}
		// expand any tRNS chunk data into a full alpha channel
		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		{
			png_set_tRNS_to_alpha(png_ptr);
		}  
		// reduce images with 16-bit samples to 8 bits
		if (img->m_bits_component == 16)
		{
			png_set_strip_16(png_ptr);            
		} 
		// expand grayscale images to RGB
		if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		{
			png_set_gray_to_rgb(png_ptr);
		}

		// read png data
		// m_bits_component will always be 8
		img->m_bits_component = 8;
		png_uint_32 rowbytes;
		png_bytep* row_pointers = (png_bytep*)malloc( sizeof(png_bytep) * img->m_height );

		png_read_update_info(png_ptr, info_ptr);

		rowbytes = png_get_rowbytes(png_ptr, info_ptr);
		assert(rowbytes == img->get_line_pitch());

		//printf("%d %d %d %d\n",rowbytes,m_height,rowbytes*m_height,get_buf_size());
		//m_buffer = new unsigned char[rowbytes * m_height];
		assert(rowbytes * img->m_height == img->get_buf_size());
		img->m_buffer = new unsigned char[img->get_buf_size()];
		CC_BREAK_IF(!img->m_buffer);

		for (unsigned short i = 0; i < img->m_height; ++i)
		{
			row_pointers[i] = img->m_buffer + i*rowbytes;
		}
		png_read_image(png_ptr, row_pointers);

		png_read_end(png_ptr, NULL);

		png_uint_32 channel = rowbytes/img->m_width;
		if (channel == 4)
		{
			img->m_alpha = true;
			unsigned int *tmp = (unsigned int *)img->m_buffer;
			for(unsigned short i = 0; i < img->m_height; i++)
			{
				for(unsigned int j = 0; j < rowbytes; j += 4)
				{
					*tmp++ = CC_RGB_PREMULTIPLY_ALPHA( row_pointers[i][j], row_pointers[i][j + 1], 
						row_pointers[i][j + 2], row_pointers[i][j + 3] );
				}
			}

			img->m_premul_alpha = true;
		}

		CC_SAFE_FREE(row_pointers);

		bRet = true;
	} while (0);

	if (png_ptr)
	{
		png_destroy_read_struct(&png_ptr, (info_ptr) ? &info_ptr : 0, 0);
	}
	return bRet;
}

