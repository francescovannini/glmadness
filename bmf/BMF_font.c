/*
    BMF_font: An OpenGL Bitmap Font Library
    Copyright (C) 2003  Trenkwalder Markus

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Trenkwalder Markus
    trenki2@gmx.net
*/

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#endif

#include <GL/gl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "BMF_font.h"


/******************************************************************************/


/* setup structure packing so that it can be loaded from file */
#pragma pack(push, 1)

typedef struct BMF_FontMetrics {
	unsigned char ptsize;
	unsigned char style;
	unsigned char height;
	unsigned char ascent;
	char descent;
	unsigned char lineskip;
} BMF_FontMetrics;

#pragma pack(pop)


struct BMF_Font {
	GLuint texture_id;
	GLuint listbase;

	BMF_FontMetrics font_metrics;
	unsigned char glyph_width[256];
};


typedef struct BMF_RenderState{
	GLfloat r, g, b, a;		/* color */
	GLfloat xpos, ypos;		/* output position */
	GLfloat xscale, yscale;
	GLfloat rotation;
	int halign, valign;		/* text alignment */

	BMF_Font* font;			/* font to be used by BMF_Print*/
} BMF_RenderState;


/* initialize the default render state render_state[0] */
static BMF_RenderState render_state[2] = {
	{
	1,	/* r */
	1,	/* g */
	1,	/* b */
	1,	/* a */
	0,	/* xpos */
	0,	/* ypos */
	1,	/* xscale */
	1,	/* yscale */
	0,	/* rotation */
	BMF_ALIGN_LEFT,		/* halign */
	BMF_ALIGN_BOTTOM,	/* valign */
	NULL				/* font */
	}
};

/* the current render state is changed when BMF_Begin() and BMF_End()
 * are executed
 */
static BMF_RenderState *rs_current = &render_state[0];

/******************************************************************************/

static void GenerateDisplayLists(BMF_Font* font, int texwidth, unsigned char nChars) {

	int i;
	int texheight = nChars == 96 ? texwidth / 2 : texwidth;

	font->listbase = glGenLists(255);

	/*glPushAttrib(GL_CURRENT_BIT);*/

	for (i = 0; i < nChars; i++ ) {

		/*Calculate the texture coordinates*/
		int left = (i % 16) * texwidth/16 + texwidth / 32 - font->glyph_width[i + ' '] / 2;
		int top = (i / 16) * texwidth/16 + texwidth / 32 - font->font_metrics.height / 2;

		int right = left + font->glyph_width[i + ' '];
		int bottom = top + font->font_metrics.height;


		/* Create the display list */
		glNewList(font->listbase +  i + ' ', GL_COMPILE);
			glBegin(GL_QUADS);
				glTexCoord2f((float)left/texwidth, (float)top/texheight);
				glVertex2f(0, font->font_metrics.height);

				glTexCoord2f((float)left/texwidth, (float)bottom/texheight);
				glVertex2f(0, 0);

				glTexCoord2f((float)right/texwidth, (float)bottom/texheight);
				glVertex2f(font->glyph_width[i + ' '], 0);

				glTexCoord2f((float)right/texwidth, (float)top/texheight);
				glVertex2f(font->glyph_width[i + ' '], font->font_metrics.height);
			glEnd();
			glTranslatef(font->glyph_width[i + ' '], 0, 0);
		glEndList();
	}

	/*glPopAttrib();	// GL_CURRENT_BIT */

}

/******************************************************************************/

static int decode_rle(unsigned char* dest, unsigned char* src, int src_size) {

	int k;

	int src_idx = 0;
	int dest_idx = 0;

	unsigned char count;
	unsigned char value;

	while ( src_idx < src_size ) {
		count = src[src_idx++];
		value = src[src_idx++];

		for ( k = 0; k < count; k++ ) {
			dest[dest_idx++] = value;
		}
	}

	return dest_idx;
}


/* Open a .bmf font file and create a font structure that can be used
   by the other functions. If an error occurs NULL is returned.
 */
BMF_Font* BMF_Load(const char* filename) {

	FILE* file = NULL;
	BMF_Font* font = NULL;

	int texturewidth;
	unsigned char* texdata = NULL;
	int rle_size;
	unsigned char *rle_data = NULL;

	unsigned int BMF_ident;
	unsigned short BMF_version;
	
	unsigned char nChars;
	unsigned int textureheight;

	#define CHECK(exp) \
		if ( !(exp) ) {	\
			free(rle_data);		\
			free(texdata);		\
			free(font);			\
			if (file) fclose(file);		\
			return NULL;		\
		}

	CHECK( file = fopen(filename, "rb") );

	/* read BMF ident and check for "BMF " exit when error */
	CHECK( fread(&BMF_ident, sizeof(BMF_ident), 1, file) );
	CHECK( BMF_ident == 0x20464D42 );

	/* read version number and check for v1.00 */
	CHECK( fread(&BMF_version, sizeof(BMF_version), 1, file) );
	CHECK( BMF_version == 0x0100 || BMF_version == 0x0101);
	
	nChars = BMF_version == 0x0100 ? 96 : 224;

	CHECK( font = (BMF_Font*)malloc(sizeof(BMF_Font)) );

	/* make sure all glyph_width values are 0 for the
	 * characters that are not used
	 */
	memset(font->glyph_width, 0, 255);

	CHECK( fread(&font->font_metrics, sizeof(BMF_FontMetrics), 1, file) );
	CHECK( fread(font->glyph_width + ' ',nChars * sizeof(unsigned char),1,file) );
	CHECK( fread(&texturewidth, sizeof(texturewidth), 1, file) );
	
	textureheight = BMF_version == 0x0100 ? texturewidth / 2 : texturewidth;
	
	/* allocate memory to hold the texture data*/
	CHECK( texdata = (unsigned char*)malloc(texturewidth * textureheight) );


	CHECK( fread(&rle_size, sizeof(rle_size), 1, file) );
	CHECK( rle_data = (unsigned char*)malloc(rle_size) );
	CHECK( fread(rle_data, rle_size, 1, file) );

	#undef CHECK

	/* store uncompressed data in texdata */
	decode_rle(texdata, rle_data, rle_size);

	glPushAttrib(GL_TEXTURE_BIT);

		glGenTextures(1, &font->texture_id);
		glBindTexture(GL_TEXTURE_2D, font->texture_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, texturewidth, textureheight, 0, GL_ALPHA, GL_UNSIGNED_BYTE, texdata);

		GenerateDisplayLists(font, texturewidth, nChars);

	glPopAttrib();

	/* clean up */
	free(rle_data);
	free(texdata);
	fclose(file);

	return font;
}

/******************************************************************************/

/* Free all resources used by the font */
void BMF_Free(BMF_Font* font) {
	if ( font != NULL ) {
		/* free the OpenGL texture */
		glDeleteTextures(1, &font->texture_id);

		/* free the display lists */
		glDeleteLists(font->listbase, 255);

		/* free the font */
		free(font);
	}
}

/******************************************************************************/

/* The function takes the font that will be used within the
   Begin() and End() pair to output text with BMF_Print().
   All the necessary GL settings are made.
   The default rendering state is set.
*/
void BMF_Begin(BMF_Font* font) {
	/* Don't execute if inside a Begin() End() pair */
	if ( rs_current != &render_state[0]) {
		return;
	}

	/* Use the default render state inside a Begin() End() pair.
	 * Changes of color, position, scaling ... will loose their effect after
	 * a call to BMF_End()
	 */

	render_state[1] = render_state[0];
	rs_current = &render_state[1];

	rs_current->font = font;


	/* Save GL state to be restored afterwards */
	glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT |
				 GL_CURRENT_BIT | GL_TEXTURE_BIT | GL_LIST_BIT | GL_DEPTH_BUFFER_BIT);

	/* make the required GL settings */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glEnable(GL_TEXTURE_2D);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	
	/* Enable alpha testing so that the completely transparent parts of the font
	 * will be discarded. This should improve rendering speed slightly. 
	 */
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);

	/* use the texture id and the fonts display list base */
	if ( font != NULL ) {
		glBindTexture(GL_TEXTURE_2D, font->texture_id);
		glListBase(font->listbase);
	}

}

/******************************************************************************/

/* Ends a BMF_Begin()/End() block and restores the GL attibutes
   that where changed by the call to BMF_Begin().
*/
void BMF_End() {
	/* Don't execute if outside a Begin() End() pair */
	if ( rs_current == &render_state[0]) {
		return;
	}

	/* outside a Begin() End() pair a default render state is used */
	rs_current = &render_state[0];

	/* restore the saved GL settings */
	glPopAttrib();
}

/******************************************************************************/

/* Print text. Use this function like the standard C printf() function.
   The currently set font, color, position, scaling, rotation, alignment ...
   is used.
   After the text has been written, the output position is advanced by
   the text width.
   If called outside BMF_Begin()/BMF_End() pairs nothing is done.
*/
void BMF_Print(const char* fmt, ...) {
	GLfloat xpos, ypos;
	char print_buffer[1024];
	
	va_list	ap;

	/* Check wheter a font is set (only possible between BMF_Begin() and
	 * BMF_End() ) there is also a text to be printed
	 */
	if ( fmt == NULL || rs_current->font == NULL) {
		return;
	}

	/* use vsprintf to parse the format string */
	va_start(ap, fmt);
	vsprintf(print_buffer, fmt, ap);
	va_end(ap);

	/* calculate the output position depending on the set text alignment */

	switch (rs_current->halign) {
	case BMF_ALIGN_CENTER:
		xpos = rs_current->xpos - BMF_GetTextWidth(rs_current->font, print_buffer) / 2;
		break;
	case BMF_ALIGN_RIGHT:
		xpos = rs_current->xpos - BMF_GetTextWidth(rs_current->font, print_buffer);
		break;
	/* BMF_ALIGN_LEFT */
	default:
		xpos = rs_current->xpos;
	}

	switch (rs_current->valign) {
	case BMF_ALIGN_TOP:
		ypos = rs_current->ypos - BMF_GetFontHeight(rs_current->font);
		break;
	case BMF_ALIGN_CENTER:
		ypos = rs_current->ypos - BMF_GetFontHeight(rs_current->font) / 2;
		break;
	/* BMF_ALIGN_BOTTOM */
	default:
		ypos = rs_current->ypos;
	}

	glPushMatrix();
		glColor4f(rs_current->r, rs_current->g, rs_current->b, rs_current->a);

		/* move to the currently set output position and rotate around that
		 * point and then move back to the calculated position */
		glLoadIdentity();
		glTranslatef(rs_current->xpos, rs_current->ypos, 0);
		glRotatef(rs_current->rotation, 0, 0, 1);
		glTranslatef(xpos - rs_current->xpos, ypos - rs_current->ypos, 0);

		glScalef(rs_current->xscale, rs_current->yscale, 1);

		glCallLists(strlen(print_buffer), GL_UNSIGNED_BYTE, print_buffer);
	glPopMatrix();

	rs_current->xpos += BMF_GetTextWidth(rs_current->font, print_buffer);
}

/******************************************************************************/

/* Set the parameters that will be used by the following calls to BMF_Print().
   If called outside a BMF_Begin() and BMF_End() pair, these functions modify
   the default rendering state that will be used when a call to BMF_Begin() is
   issued.
*/

void BMF_SetColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
	rs_current->r = r;
	rs_current->g = g;
	rs_current->b = b;
	rs_current->a = a;
}

void BMF_SetPos(GLfloat x, GLfloat y) {
	rs_current->xpos = x;
	rs_current->ypos = y;
}

void BMF_SetScale(GLfloat xs, GLfloat ys) {
	rs_current->xscale = xs;
	rs_current->yscale = ys;
}

void BMF_SetRotation(GLfloat degrees) {
	rs_current->rotation = degrees;
}

void BMF_SetVAlign(int style) {
	rs_current->valign = style;
}

void BMF_SetHAlign(int style) {
	rs_current->halign = style;
}

/******************************************************************************/

/* Returns the width of the text for a given font.
   The currently set scaling factor is taken into account.
*/
GLfloat BMF_GetTextWidth(BMF_Font* font, const char* text) {

	unsigned int i;
	GLfloat width = 0;

	if ( text == NULL ) {
		return 0;
	}

	for ( i = 0; i < strlen(text); i++ ) {
		width += font->glyph_width[(unsigned char)text[i]];
	}

	return width * rs_current->xscale;
}

/******************************************************************************/

/* Returns the height of the font.
   The currently set scalig factor is taken into account.
*/
GLfloat BMF_GetFontHeight(BMF_Font* font) {
	return rs_current->yscale * font->font_metrics.height;
}

/******************************************************************************/

/* Returns the value lineskip.
   The currently set scaling factor is taken into account.
*/
GLfloat BMF_GetLineSkip(BMF_Font* font) {
	return rs_current->yscale * font->font_metrics.lineskip;
}

/******************************************************************************/
