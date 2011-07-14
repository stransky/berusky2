/*
  Rozsireni pro open gl
*/

#include "3d_all.h"

void glDefaultFunkce(void)
{
}

int extlist_multitexture;
int extlist_blend_color;
int extlist_blend_minmax;
int extlist_fog_coord;
int extlist_secondary_color;
int extlist_fence;
int extlist_register_combiners;
int extlist_text_compresion;
int extlist_text_sharp;
int extlist_text_edge_clamp;
int extlist_text_env_combine;
int extlist_text_env_dot3;
int extlist_text_filter_anisotropic;
int extlist_vertex_array_range;
int extlist_vertex_array;
int extlist_indicie_array;
int extlist_vertex_array_packed;
int extlist_clip_volume_hit;
int extlist_ati_env_bump;
int extlist_ati_env_bump_unit[6];
int extlist_ati_vertex_array_object;
int extlist_ati_element_array;
int extlist_ati_pn_triangles;
int extlist_arb_vertex_buffer;
int extlist_ati_draw_buffers;

/*
void (APIENTRY * glBlendColor) (GLclampf, GLclampf, GLclampf, GLclampf);
void (APIENTRY * glBlendEquation) (GLenum);
void (APIENTRY * glDrawRangeElements) (GLenum, GLuint, GLuint, GLsizei, GLenum, const GLvoid *);
void (APIENTRY * glColorTable) (GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid *);
void (APIENTRY * glColorTableParameterfv) (GLenum, GLenum, const GLfloat *);
void (APIENTRY * glColorTableParameteriv) (GLenum, GLenum, const GLint *);
void (APIENTRY * glCopyColorTable) (GLenum, GLenum, GLint, GLint, GLsizei);
void (APIENTRY * glGetColorTable) (GLenum, GLenum, GLenum, GLvoid *);
void (APIENTRY * glGetColorTableParameterfv) (GLenum, GLenum, GLfloat *);
void (APIENTRY * glGetColorTableParameteriv) (GLenum, GLenum, GLint *);
void (APIENTRY * glColorSubTable) (GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *);
void (APIENTRY * glCopyColorSubTable) (GLenum, GLsizei, GLint, GLint, GLsizei);
void (APIENTRY * glConvolutionFilter1D) (GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid *);
void (APIENTRY * glConvolutionFilter2D) (GLenum, GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *);
void (APIENTRY * glConvolutionParameterf) (GLenum, GLenum, GLfloat);
void (APIENTRY * glConvolutionParameterfv) (GLenum, GLenum, const GLfloat *);
void (APIENTRY * glConvolutionParameteri) (GLenum, GLenum, GLint);
void (APIENTRY * glConvolutionParameteriv) (GLenum, GLenum, const GLint *);
void (APIENTRY * glCopyConvolutionFilter1D) (GLenum, GLenum, GLint, GLint, GLsizei);
void (APIENTRY * glCopyConvolutionFilter2D) (GLenum, GLenum, GLint, GLint, GLsizei, GLsizei);
void (APIENTRY * glGetConvolutionFilter) (GLenum, GLenum, GLenum, GLvoid *);
void (APIENTRY * glGetConvolutionParameterfv) (GLenum, GLenum, GLfloat *);
void (APIENTRY * glGetConvolutionParameteriv) (GLenum, GLenum, GLint *);
void (APIENTRY * glGetSeparableFilter) (GLenum, GLenum, GLenum, GLvoid *, GLvoid *, GLvoid *);
void (APIENTRY * glSeparableFilter2D) (GLenum, GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *, const GLvoid *);
void (APIENTRY * glGetHistogram) (GLenum, GLboolean, GLenum, GLenum, GLvoid *);
void (APIENTRY * glGetHistogramParameterfv) (GLenum, GLenum, GLfloat *);
void (APIENTRY * glGetHistogramParameteriv) (GLenum, GLenum, GLint *);
void (APIENTRY * glGetMinmax) (GLenum, GLboolean, GLenum, GLenum, GLvoid *);
void (APIENTRY * glGetMinmaxParameterfv) (GLenum, GLenum, GLfloat *);
void (APIENTRY * glGetMinmaxParameteriv) (GLenum, GLenum, GLint *);
void (APIENTRY * glHistogram) (GLenum, GLsizei, GLenum, GLboolean);
void (APIENTRY * glMinmax) (GLenum, GLenum, GLboolean);
void (APIENTRY * glResetHistogram) (GLenum);
void (APIENTRY * glResetMinmax) (GLenum);
void (APIENTRY * glTexImage3D) (GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *);
void (APIENTRY * glTexSubImage3D) (GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *);
void (APIENTRY * glCopyTexSubImage3D) (GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei);
 
void (APIENTRY * glActiveTextureARB) (GLenum);
void (APIENTRY * glClientActiveTextureARB) (GLenum);
void (APIENTRY * glMultiTexCoord1dARB) (GLenum, GLdouble);
void (APIENTRY * glMultiTexCoord1dvARB) (GLenum, const GLdouble *);
void (APIENTRY * glMultiTexCoord1fARB) (GLenum, GLfloat);
void (APIENTRY * glMultiTexCoord1fvARB) (GLenum, const GLfloat *);
void (APIENTRY * glMultiTexCoord1iARB) (GLenum, GLint);
void (APIENTRY * glMultiTexCoord1ivARB) (GLenum, const GLint *);
void (APIENTRY * glMultiTexCoord1sARB) (GLenum, GLshort);
void (APIENTRY * glMultiTexCoord1svARB) (GLenum, const GLshort *);
void (APIENTRY * glMultiTexCoord2dARB) (GLenum, GLdouble, GLdouble);
void (APIENTRY * glMultiTexCoord2dvARB) (GLenum, const GLdouble *);
void (APIENTRY * glMultiTexCoord2fARB) (GLenum, GLfloat, GLfloat);
void (APIENTRY * glMultiTexCoord2fvARB) (GLenum, const GLfloat *);
void (APIENTRY * glMultiTexCoord2iARB) (GLenum, GLint, GLint);
void (APIENTRY * glMultiTexCoord2ivARB) (GLenum, const GLint *);
void (APIENTRY * glMultiTexCoord2sARB) (GLenum, GLshort, GLshort);
void (APIENTRY * glMultiTexCoord2svARB) (GLenum, const GLshort *);
void (APIENTRY * glMultiTexCoord3dARB) (GLenum, GLdouble, GLdouble, GLdouble);
void (APIENTRY * glMultiTexCoord3dvARB) (GLenum, const GLdouble *);
void (APIENTRY * glMultiTexCoord3fARB) (GLenum, GLfloat, GLfloat, GLfloat);
void (APIENTRY * glMultiTexCoord3fvARB) (GLenum, const GLfloat *);
void (APIENTRY * glMultiTexCoord3iARB) (GLenum, GLint, GLint, GLint);
void (APIENTRY * glMultiTexCoord3ivARB) (GLenum, const GLint *);
void (APIENTRY * glMultiTexCoord3sARB) (GLenum, GLshort, GLshort, GLshort);
void (APIENTRY * glMultiTexCoord3svARB) (GLenum, const GLshort *);
void (APIENTRY * glMultiTexCoord4dARB) (GLenum, GLdouble, GLdouble, GLdouble, GLdouble);
void (APIENTRY * glMultiTexCoord4dvARB) (GLenum, const GLdouble *);
void (APIENTRY * glMultiTexCoord4fARB) (GLenum, GLfloat, GLfloat, GLfloat, GLfloat);
void (APIENTRY * glMultiTexCoord4fvARB) (GLenum, const GLfloat *);
void (APIENTRY * glMultiTexCoord4iARB) (GLenum, GLint, GLint, GLint, GLint);
void (APIENTRY * glMultiTexCoord4ivARB) (GLenum, const GLint *);
void (APIENTRY * glMultiTexCoord4sARB) (GLenum, GLshort, GLshort, GLshort, GLshort);
void (APIENTRY * glMultiTexCoord4svARB) (GLenum, const GLshort *);

void (APIENTRY * glLoadTransposeMatrixfARB) (const GLfloat *);
void (APIENTRY * glLoadTransposeMatrixdARB) (const GLdouble *);
void (APIENTRY * glMultTransposeMatrixfARB) (const GLfloat *);
void (APIENTRY * glMultTransposeMatrixdARB) (const GLdouble *);


void (APIENTRY * glSampleCoverageARB) (GLclampf, GLboolean);
void (APIENTRY * glSamplePassARB) (GLenum);


void (APIENTRY * glCompressedTexImage3DARB) (GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *);
void (APIENTRY * glCompressedTexImage2DARB) (GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *);
void (APIENTRY * glCompressedTexImage1DARB) (GLenum, GLint, GLenum, GLsizei, GLint, GLsizei, const GLvoid *);
void (APIENTRY * glCompressedTexSubImage3DARB) (GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *);
void (APIENTRY * glCompressedTexSubImage2DARB) (GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *);
void (APIENTRY * glCompressedTexSubImage1DARB) (GLenum, GLint, GLint, GLsizei, GLenum, GLsizei, const GLvoid *);
void (APIENTRY * glGetCompressedTexImageARB) (GLenum, GLint,void *);


void (APIENTRY * glBlendColorEXT) (GLclampf, GLclampf, GLclampf, GLclampf);


void (APIENTRY * glPolygonOffsetEXT) (GLfloat, GLfloat);


void (APIENTRY * glTexImage3DEXT) (GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *);
void (APIENTRY * glTexSubImage3DEXT) (GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *);


void (APIENTRY * glGetTexFilterFuncSGIS) (GLenum, GLenum, GLfloat *);
void (APIENTRY * glTexFilterFuncSGIS) (GLenum, GLenum, GLsizei, const GLfloat *);


void (APIENTRY * glTexSubImage1DEXT) (GLenum, GLint, GLint, GLsizei, GLenum, GLenum, const GLvoid *);
void (APIENTRY * glTexSubImage2DEXT) (GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *);


void (APIENTRY * glCopyTexImage1DEXT) (GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLint);
void (APIENTRY * glCopyTexImage2DEXT) (GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint);
void (APIENTRY * glCopyTexSubImage1DEXT) (GLenum, GLint, GLint, GLint, GLint, GLsizei);
void (APIENTRY * glCopyTexSubImage2DEXT) (GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei);
void (APIENTRY * glCopyTexSubImage3DEXT) (GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei);


void (APIENTRY * glGetHistogramEXT) (GLenum, GLboolean, GLenum, GLenum, GLvoid *);
void (APIENTRY * glGetHistogramParameterfvEXT) (GLenum, GLenum, GLfloat *);
void (APIENTRY * glGetHistogramParameterivEXT) (GLenum, GLenum, GLint *);
void (APIENTRY * glGetMinmaxEXT) (GLenum, GLboolean, GLenum, GLenum, GLvoid *);
void (APIENTRY * glGetMinmaxParameterfvEXT) (GLenum, GLenum, GLfloat *);
void (APIENTRY * glGetMinmaxParameterivEXT) (GLenum, GLenum, GLint *);
void (APIENTRY * glHistogramEXT) (GLenum, GLsizei, GLenum, GLboolean);
void (APIENTRY * glMinmaxEXT) (GLenum, GLenum, GLboolean);
void (APIENTRY * glResetHistogramEXT) (GLenum);
void (APIENTRY * glResetMinmaxEXT) (GLenum);


void (APIENTRY * glConvolutionFilter1DEXT) (GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid *);
void (APIENTRY * glConvolutionFilter2DEXT) (GLenum, GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *);
void (APIENTRY * glConvolutionParameterfEXT) (GLenum, GLenum, GLfloat);
void (APIENTRY * glConvolutionParameterfvEXT) (GLenum, GLenum, const GLfloat *);
void (APIENTRY * glConvolutionParameteriEXT) (GLenum, GLenum, GLint);
void (APIENTRY * glConvolutionParameterivEXT) (GLenum, GLenum, const GLint *);
void (APIENTRY * glCopyConvolutionFilter1DEXT) (GLenum, GLenum, GLint, GLint, GLsizei);
void (APIENTRY * glCopyConvolutionFilter2DEXT) (GLenum, GLenum, GLint, GLint, GLsizei, GLsizei);
void (APIENTRY * glGetConvolutionFilterEXT) (GLenum, GLenum, GLenum, GLvoid *);
void (APIENTRY * glGetConvolutionParameterfvEXT) (GLenum, GLenum, GLfloat *);
void (APIENTRY * glGetConvolutionParameterivEXT) (GLenum, GLenum, GLint *);
void (APIENTRY * glGetSeparableFilterEXT) (GLenum, GLenum, GLenum, GLvoid *, GLvoid *, GLvoid *);
void (APIENTRY * glSeparableFilter2DEXT) (GLenum, GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *, const GLvoid *);

void (APIENTRY * glColorTableSGI) (GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid *);
void (APIENTRY * glColorTableParameterfvSGI) (GLenum, GLenum, const GLfloat *);
void (APIENTRY * glColorTableParameterivSGI) (GLenum, GLenum, const GLint *);
void (APIENTRY * glCopyColorTableSGI) (GLenum, GLenum, GLint, GLint, GLsizei);
void (APIENTRY * glGetColorTableSGI) (GLenum, GLenum, GLenum, GLvoid *);
void (APIENTRY * glGetColorTableParameterfvSGI) (GLenum, GLenum, GLfloat *);
void (APIENTRY * glGetColorTableParameterivSGI) (GLenum, GLenum, GLint *);

void (APIENTRY * glPixelTexGenSGIX) (GLenum);

void (APIENTRY * glPixelTexGenParameteriSGIS) (GLenum, GLint);
void (APIENTRY * glPixelTexGenParameterivSGIS) (GLenum, const GLint *);
void (APIENTRY * glPixelTexGenParameterfSGIS) (GLenum, GLfloat);
void (APIENTRY * glPixelTexGenParameterfvSGIS) (GLenum, const GLfloat *);
void (APIENTRY * glGetPixelTexGenParameterivSGIS) (GLenum, GLint *);
void (APIENTRY * glGetPixelTexGenParameterfvSGIS) (GLenum, GLfloat *);

void (APIENTRY * glTexImage4DSGIS) (GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *);
void (APIENTRY * glTexSubImage4DSGIS) (GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *);


 GLboolean (APIENTRY * glAreTexturesResidentEXT) (GLsizei, const GLuint *, GLboolean *);
void (APIENTRY * glBindTextureEXT) (GLenum, GLuint);
void (APIENTRY * glDeleteTexturesEXT) (GLsizei, const GLuint *);
void (APIENTRY * glGenTexturesEXT) (GLsizei, GLuint *);
 GLboolean (APIENTRY * glIsTextureEXT) (GLuint);
void (APIENTRY * glPrioritizeTexturesEXT) (GLsizei, const GLuint *, const GLclampf *);

void (APIENTRY * glDetailTexFuncSGIS) (GLenum, GLsizei, const GLfloat *);
void (APIENTRY * glGetDetailTexFuncSGIS) (GLenum, GLfloat *);

void (APIENTRY * glSharpenTexFuncSGIS) (GLenum, GLsizei, const GLfloat *);
void (APIENTRY * glGetSharpenTexFuncSGIS) (GLenum, GLfloat *);

void (APIENTRY * glSampleMaskSGIS) (GLclampf, GLboolean);
void (APIENTRY * glSamplePatternSGIS) (GLenum);

void (APIENTRY * glArrayElementEXT) (GLint);
void (APIENTRY * glColorPointerEXT) (GLint, GLenum, GLsizei, GLsizei, const GLvoid *);
void (APIENTRY * glDrawArraysEXT) (GLenum, GLint, GLsizei);
void (APIENTRY * glEdgeFlagPointerEXT) (GLsizei, GLsizei, const GLboolean *);
void (APIENTRY * glGetPointervEXT) (GLenum, GLvoid* *);
void (APIENTRY * glIndexPointerEXT) (GLenum, GLsizei, GLsizei, const GLvoid *);
void (APIENTRY * glNormalPointerEXT) (GLenum, GLsizei, GLsizei, const GLvoid *);
void (APIENTRY * glTexCoordPointerEXT) (GLint, GLenum, GLsizei, GLsizei, const GLvoid *);
void (APIENTRY * glVertexPointerEXT) (GLint, GLenum, GLsizei, GLsizei, const GLvoid *);

void (APIENTRY * glBlendEquationEXT) (GLenum);

void (APIENTRY * glSpriteParameterfSGIX) (GLenum, GLfloat);
void (APIENTRY * glSpriteParameterfvSGIX) (GLenum, const GLfloat *);
void (APIENTRY * glSpriteParameteriSGIX) (GLenum, GLint);
void (APIENTRY * glSpriteParameterivSGIX) (GLenum, const GLint *);

void (APIENTRY * glPointParameterfEXT) (GLenum, GLfloat);
void (APIENTRY * glPointParameterfvEXT) (GLenum, const GLfloat *);
void (APIENTRY * glPointParameterfSGIS) (GLenum, GLfloat);
void (APIENTRY * glPointParameterfvSGIS) (GLenum, const GLfloat *);

 GLint (APIENTRY * glGetInstrumentsSGIX) (void);
void (APIENTRY * glInstrumentsBufferSGIX) (GLsizei, GLint *);
 GLint (APIENTRY * glPollInstrumentsSGIX) (GLint *);
void (APIENTRY * glReadInstrumentsSGIX) (GLint);
void (APIENTRY * glStartInstrumentsSGIX) (void);
void (APIENTRY * glStopInstrumentsSGIX) (GLint);

void (APIENTRY * glFrameZoomSGIX) (GLint);

void (APIENTRY * glTagSampleBufferSGIX) (void);

void (APIENTRY * glReferencePlaneSGIX) (const GLdouble *);

void (APIENTRY * glFlushRasterSGIX) (void);

void (APIENTRY * glFogFuncSGIS) (GLsizei, const GLfloat *);
void (APIENTRY * glGetFogFuncSGIS) (const GLfloat *);

void (APIENTRY * glImageTransformParameteriHP) (GLenum, GLenum, GLint);
void (APIENTRY * glImageTransformParameterfHP) (GLenum, GLenum, GLfloat);
void (APIENTRY * glImageTransformParameterivHP) (GLenum, GLenum, const GLint *);
void (APIENTRY * glImageTransformParameterfvHP) (GLenum, GLenum, const GLfloat *);
void (APIENTRY * glGetImageTransformParameterivHP) (GLenum, GLenum, GLint *);
void (APIENTRY * glGetImageTransformParameterfvHP) (GLenum, GLenum, GLfloat *);

void (APIENTRY * glColorSubTableEXT) (GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *);
void (APIENTRY * glCopyColorSubTableEXT) (GLenum, GLsizei, GLint, GLint, GLsizei);

void (APIENTRY * glHintPGI) (GLenum, GLint);

void (APIENTRY * glColorTableEXT) (GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid *);
void (APIENTRY * glGetColorTableEXT) (GLenum, GLenum, GLenum, GLvoid *);
void (APIENTRY * glGetColorTableParameterivEXT) (GLenum, GLenum, GLint *);
void (APIENTRY * glGetColorTableParameterfvEXT) (GLenum, GLenum, GLfloat *);

void (APIENTRY * glGetListParameterfvSGIX) (GLuint, GLenum, GLfloat *);
void (APIENTRY * glGetListParameterivSGIX) (GLuint, GLenum, GLint *);
void (APIENTRY * glListParameterfSGIX) (GLuint, GLenum, GLfloat);
void (APIENTRY * glListParameterfvSGIX) (GLuint, GLenum, const GLfloat *);
void (APIENTRY * glListParameteriSGIX) (GLuint, GLenum, GLint);
void (APIENTRY * glListParameterivSGIX) (GLuint, GLenum, const GLint *);

void (APIENTRY * glIndexMaterialEXT) (GLenum, GLenum);

void (APIENTRY * glIndexFuncEXT) (GLenum, GLclampf);

void (APIENTRY * glLockArraysEXT) (GLint, GLsizei);
void (APIENTRY * glUnlockArraysEXT) (void);

void (APIENTRY * glCullParameterdvEXT) (GLenum, GLdouble *);
void (APIENTRY * glCullParameterfvEXT) (GLenum, GLfloat *);

void (APIENTRY * glFragmentColorMaterialSGIX) (GLenum, GLenum);
void (APIENTRY * glFragmentLightfSGIX) (GLenum, GLenum, GLfloat);
void (APIENTRY * glFragmentLightfvSGIX) (GLenum, GLenum, const GLfloat *);
void (APIENTRY * glFragmentLightiSGIX) (GLenum, GLenum, GLint);
void (APIENTRY * glFragmentLightivSGIX) (GLenum, GLenum, const GLint *);
void (APIENTRY * glFragmentLightModelfSGIX) (GLenum, GLfloat);
void (APIENTRY * glFragmentLightModelfvSGIX) (GLenum, const GLfloat *);
void (APIENTRY * glFragmentLightModeliSGIX) (GLenum, GLint);
void (APIENTRY * glFragmentLightModelivSGIX) (GLenum, const GLint *);
void (APIENTRY * glFragmentMaterialfSGIX) (GLenum, GLenum, GLfloat);
void (APIENTRY * glFragmentMaterialfvSGIX) (GLenum, GLenum, const GLfloat *);
void (APIENTRY * glFragmentMaterialiSGIX) (GLenum, GLenum, GLint);
void (APIENTRY * glFragmentMaterialivSGIX) (GLenum, GLenum, const GLint *);
void (APIENTRY * glGetFragmentLightfvSGIX) (GLenum, GLenum, GLfloat *);
void (APIENTRY * glGetFragmentLightivSGIX) (GLenum, GLenum, GLint *);
void (APIENTRY * glGetFragmentMaterialfvSGIX) (GLenum, GLenum, GLfloat *);
void (APIENTRY * glGetFragmentMaterialivSGIX) (GLenum, GLenum, GLint *);
void (APIENTRY * glLightEnviSGIX) (GLenum, GLint);

void (APIENTRY * glDrawRangeElementsEXT) (GLenum, GLuint, GLuint, GLsizei, GLenum, const GLvoid *);

void (APIENTRY * glApplyTextureEXT) (GLenum);
void (APIENTRY * glTextureLightEXT) (GLenum);
void (APIENTRY * glTextureMaterialEXT) (GLenum, GLenum);

void (APIENTRY * glVertexPointervINTEL) (GLint, GLenum, const GLvoid* *);
void (APIENTRY * glNormalPointervINTEL) (GLenum, const GLvoid* *);
void (APIENTRY * glColorPointervINTEL) (GLint, GLenum, const GLvoid* *);
void (APIENTRY * glTexCoordPointervINTEL) (GLint, GLenum, const GLvoid* *);

void (APIENTRY * glPixelTransformParameteriEXT) (GLenum, GLenum, GLint);
void (APIENTRY * glPixelTransformParameterfEXT) (GLenum, GLenum, GLfloat);
void (APIENTRY * glPixelTransformParameterivEXT) (GLenum, GLenum, const GLint *);
void (APIENTRY * glPixelTransformParameterfvEXT) (GLenum, GLenum, const GLfloat *);

void (APIENTRY * glSecondaryColor3bEXT) (GLbyte, GLbyte, GLbyte);
void (APIENTRY * glSecondaryColor3bvEXT) (const GLbyte *);
void (APIENTRY * glSecondaryColor3dEXT) (GLdouble, GLdouble, GLdouble);
void (APIENTRY * glSecondaryColor3dvEXT) (const GLdouble *);
void (APIENTRY * glSecondaryColor3fEXT) (GLfloat, GLfloat, GLfloat);
void (APIENTRY * glSecondaryColor3fvEXT) (const GLfloat *);
void (APIENTRY * glSecondaryColor3iEXT) (GLint, GLint, GLint);
void (APIENTRY * glSecondaryColor3ivEXT) (const GLint *);
void (APIENTRY * glSecondaryColor3sEXT) (GLshort, GLshort, GLshort);
void (APIENTRY * glSecondaryColor3svEXT) (const GLshort *);
void (APIENTRY * glSecondaryColor3ubEXT) (GLubyte, GLubyte, GLubyte);
void (APIENTRY * glSecondaryColor3ubvEXT) (const GLubyte *);
void (APIENTRY * glSecondaryColor3uiEXT) (GLuint, GLuint, GLuint);
void (APIENTRY * glSecondaryColor3uivEXT) (const GLuint *);
void (APIENTRY * glSecondaryColor3usEXT) (GLushort, GLushort, GLushort);
void (APIENTRY * glSecondaryColor3usvEXT) (const GLushort *);
void (APIENTRY * glSecondaryColorPointerEXT) (GLint, GLenum, GLsizei, GLvoid *);

void (APIENTRY * glTextureNormalEXT) (GLenum);

void (APIENTRY * glMultiDrawArraysEXT) (GLenum, GLint *, GLsizei *, GLsizei);
void (APIENTRY * glMultiDrawElementsEXT) (GLenum, const GLsizei *, GLenum, const GLvoid* *, GLsizei);

void (APIENTRY * glFogCoordfEXT) (GLfloat);
void (APIENTRY * glFogCoordfvEXT) (const GLfloat *);
void (APIENTRY * glFogCoorddEXT) (GLdouble);
void (APIENTRY * glFogCoorddvEXT) (const GLdouble *);
void (APIENTRY * glFogCoordPointerEXT) (GLenum, GLsizei, const GLvoid *);

void (APIENTRY * glTangent3bEXT) (GLbyte, GLbyte, GLbyte);
void (APIENTRY * glTangent3bvEXT) (const GLbyte *);
void (APIENTRY * glTangent3dEXT) (GLdouble, GLdouble, GLdouble);
void (APIENTRY * glTangent3dvEXT) (const GLdouble *);
void (APIENTRY * glTangent3fEXT) (GLfloat, GLfloat, GLfloat);
void (APIENTRY * glTangent3fvEXT) (const GLfloat *);
void (APIENTRY * glTangent3iEXT) (GLint, GLint, GLint);
void (APIENTRY * glTangent3ivEXT) (const GLint *);
void (APIENTRY * glTangent3sEXT) (GLshort, GLshort, GLshort);
void (APIENTRY * glTangent3svEXT) (const GLshort *);
void (APIENTRY * glBinormal3bEXT) (GLbyte, GLbyte, GLbyte);
void (APIENTRY * glBinormal3bvEXT) (const GLbyte *);
void (APIENTRY * glBinormal3dEXT) (GLdouble, GLdouble, GLdouble);
void (APIENTRY * glBinormal3dvEXT) (const GLdouble *);
void (APIENTRY * glBinormal3fEXT) (GLfloat, GLfloat, GLfloat);
void (APIENTRY * glBinormal3fvEXT) (const GLfloat *);
void (APIENTRY * glBinormal3iEXT) (GLint, GLint, GLint);
void (APIENTRY * glBinormal3ivEXT) (const GLint *);
void (APIENTRY * glBinormal3sEXT) (GLshort, GLshort, GLshort);
void (APIENTRY * glBinormal3svEXT) (const GLshort *);
void (APIENTRY * glTangentPointerEXT) (GLenum, GLsizei, const GLvoid *);
void (APIENTRY * glBinormalPointerEXT) (GLenum, GLsizei, const GLvoid *);

void (APIENTRY * glFinishTextureSUNX) (void);

void (APIENTRY * glGlobalAlphaFactorbSUN) (GLbyte);
void (APIENTRY * glGlobalAlphaFactorsSUN) (GLshort);
void (APIENTRY * glGlobalAlphaFactoriSUN) (GLint);
void (APIENTRY * glGlobalAlphaFactorfSUN) (GLfloat);
void (APIENTRY * glGlobalAlphaFactordSUN) (GLdouble);
void (APIENTRY * glGlobalAlphaFactorubSUN) (GLubyte);
void (APIENTRY * glGlobalAlphaFactorusSUN) (GLushort);
void (APIENTRY * glGlobalAlphaFactoruiSUN) (GLuint);

void (APIENTRY * glReplacementCodeuiSUN) (GLuint);
void (APIENTRY * glReplacementCodeusSUN) (GLushort);
void (APIENTRY * glReplacementCodeubSUN) (GLubyte);
void (APIENTRY * glReplacementCodeuivSUN) (const GLuint *);
void (APIENTRY * glReplacementCodeusvSUN) (const GLushort *);
void (APIENTRY * glReplacementCodeubvSUN) (const GLubyte *);
void (APIENTRY * glReplacementCodePointerSUN) (GLenum, GLsizei, const GLvoid* *);

void (APIENTRY * glColor4ubVertex2fSUN) (GLubyte, GLubyte, GLubyte, GLubyte, GLfloat, GLfloat);
void (APIENTRY * glColor4ubVertex2fvSUN) (const GLubyte *, const GLfloat *);
void (APIENTRY * glColor4ubVertex3fSUN) (GLubyte, GLubyte, GLubyte, GLubyte, GLfloat, GLfloat, GLfloat);
void (APIENTRY * glColor4ubVertex3fvSUN) (const GLubyte *, const GLfloat *);
void (APIENTRY * glColor3fVertex3fSUN) (GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
void (APIENTRY * glColor3fVertex3fvSUN) (const GLfloat *, const GLfloat *);
void (APIENTRY * glNormal3fVertex3fSUN) (GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
void (APIENTRY * glNormal3fVertex3fvSUN) (const GLfloat *, const GLfloat *);
void (APIENTRY * glColor4fNormal3fVertex3fSUN) (GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
void (APIENTRY * glColor4fNormal3fVertex3fvSUN) (const GLfloat *, const GLfloat *, const GLfloat *);
void (APIENTRY * glTexCoord2fVertex3fSUN) (GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
void (APIENTRY * glTexCoord2fVertex3fvSUN) (const GLfloat *, const GLfloat *);
void (APIENTRY * glTexCoord4fVertex4fSUN) (GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
void (APIENTRY * glTexCoord4fVertex4fvSUN) (const GLfloat *, const GLfloat *);
void (APIENTRY * glTexCoord2fColor4ubVertex3fSUN) (GLfloat, GLfloat, GLubyte, GLubyte, GLubyte, GLubyte, GLfloat, GLfloat, GLfloat);
void (APIENTRY * glTexCoord2fColor4ubVertex3fvSUN) (const GLfloat *, const GLubyte *, const GLfloat *);
void (APIENTRY * glTexCoord2fColor3fVertex3fSUN) (GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
void (APIENTRY * glTexCoord2fColor3fVertex3fvSUN) (const GLfloat *, const GLfloat *, const GLfloat *);
void (APIENTRY * glTexCoord2fNormal3fVertex3fSUN) (GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
void (APIENTRY * glTexCoord2fNormal3fVertex3fvSUN) (const GLfloat *, const GLfloat *, const GLfloat *);
void (APIENTRY * glTexCoord2fColor4fNormal3fVertex3fSUN) (GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
void (APIENTRY * glTexCoord2fColor4fNormal3fVertex3fvSUN) (const GLfloat *, const GLfloat *, const GLfloat *, const GLfloat *);
void (APIENTRY * glTexCoord4fColor4fNormal3fVertex4fSUN) (GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
void (APIENTRY * glTexCoord4fColor4fNormal3fVertex4fvSUN) (const GLfloat *, const GLfloat *, const GLfloat *, const GLfloat *);
void (APIENTRY * glReplacementCodeuiVertex3fSUN) (GLenum, GLfloat, GLfloat, GLfloat);
void (APIENTRY * glReplacementCodeuiVertex3fvSUN) (const GLenum *, const GLfloat *);
void (APIENTRY * glReplacementCodeuiColor4ubVertex3fSUN) (GLenum, GLubyte, GLubyte, GLubyte, GLubyte, GLfloat, GLfloat, GLfloat);
void (APIENTRY * glReplacementCodeuiColor4ubVertex3fvSUN) (const GLenum *, const GLubyte *, const GLfloat *);
void (APIENTRY * glReplacementCodeuiColor3fVertex3fSUN) (GLenum, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
void (APIENTRY * glReplacementCodeuiColor3fVertex3fvSUN) (const GLenum *, const GLfloat *, const GLfloat *);
void (APIENTRY * glReplacementCodeuiNormal3fVertex3fSUN) (GLenum, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
void (APIENTRY * glReplacementCodeuiNormal3fVertex3fvSUN) (const GLenum *, const GLfloat *, const GLfloat *);
void (APIENTRY * glReplacementCodeuiColor4fNormal3fVertex3fSUN) (GLenum, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
void (APIENTRY * glReplacementCodeuiColor4fNormal3fVertex3fvSUN) (const GLenum *, const GLfloat *, const GLfloat *, const GLfloat *);
void (APIENTRY * glReplacementCodeuiTexCoord2fVertex3fSUN) (GLenum, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
void (APIENTRY * glReplacementCodeuiTexCoord2fVertex3fvSUN) (const GLenum *, const GLfloat *, const GLfloat *);
void (APIENTRY * glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN) (GLenum, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
void (APIENTRY * glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN) (const GLenum *, const GLfloat *, const GLfloat *, const GLfloat *);
void (APIENTRY * glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN) (GLenum, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
void (APIENTRY * glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN) (const GLenum *, const GLfloat *, const GLfloat *, const GLfloat *, const GLfloat *);

void (APIENTRY * glBlendFuncSeparateEXT) (GLenum, GLenum, GLenum, GLenum);

void (APIENTRY * glVertexWeightfEXT) (GLfloat);
void (APIENTRY * glVertexWeightfvEXT) (const GLfloat *);
void (APIENTRY * glVertexWeightPointerEXT) (GLsizei, GLenum, GLsizei, const GLvoid *);

void (APIENTRY * glFlushVertexArrayRangeNV) (void);
void (APIENTRY * glVertexArrayRangeNV) (GLsizei, const GLvoid *);

void (APIENTRY * glCombinerParameterfvNV) (GLenum, const GLfloat *);
void (APIENTRY * glCombinerParameterfNV) (GLenum, GLfloat);
void (APIENTRY * glCombinerParameterivNV) (GLenum, const GLint *);
void (APIENTRY * glCombinerParameteriNV) (GLenum, GLint);
void (APIENTRY * glCombinerInputNV) (GLenum, GLenum, GLenum, GLenum, GLenum, GLenum);
void (APIENTRY * glCombinerOutputNV) (GLenum, GLenum, GLenum, GLenum, GLenum, GLenum, GLenum, GLboolean, GLboolean, GLboolean);
void (APIENTRY * glFinalCombinerInputNV) (GLenum, GLenum, GLenum, GLenum);
void (APIENTRY * glGetCombinerInputParameterfvNV) (GLenum, GLenum, GLenum, GLenum, GLfloat *);
void (APIENTRY * glGetCombinerInputParameterivNV) (GLenum, GLenum, GLenum, GLenum, GLint *);
void (APIENTRY * glGetCombinerOutputParameterfvNV) (GLenum, GLenum, GLenum, GLfloat *);
void (APIENTRY * glGetCombinerOutputParameterivNV) (GLenum, GLenum, GLenum, GLint *);
void (APIENTRY * glGetFinalCombinerInputParameterfvNV) (GLenum, GLenum, GLfloat *);
void (APIENTRY * glGetFinalCombinerInputParameterivNV) (GLenum, GLenum, GLint *);

void (APIENTRY * glResizeBuffersMESA) (void);

void (APIENTRY * glWindowPos2dMESA) (GLdouble, GLdouble);
void (APIENTRY * glWindowPos2dvMESA) (const GLdouble *);
void (APIENTRY * glWindowPos2fMESA) (GLfloat, GLfloat);
void (APIENTRY * glWindowPos2fvMESA) (const GLfloat *);
void (APIENTRY * glWindowPos2iMESA) (GLint, GLint);
void (APIENTRY * glWindowPos2ivMESA) (const GLint *);
void (APIENTRY * glWindowPos2sMESA) (GLshort, GLshort);
void (APIENTRY * glWindowPos2svMESA) (const GLshort *);
void (APIENTRY * glWindowPos3dMESA) (GLdouble, GLdouble, GLdouble);
void (APIENTRY * glWindowPos3dvMESA) (const GLdouble *);
void (APIENTRY * glWindowPos3fMESA) (GLfloat, GLfloat, GLfloat);
void (APIENTRY * glWindowPos3fvMESA) (const GLfloat *);
void (APIENTRY * glWindowPos3iMESA) (GLint, GLint, GLint);
void (APIENTRY * glWindowPos3ivMESA) (const GLint *);
void (APIENTRY * glWindowPos3sMESA) (GLshort, GLshort, GLshort);
void (APIENTRY * glWindowPos3svMESA) (const GLshort *);
void (APIENTRY * glWindowPos4dMESA) (GLdouble, GLdouble, GLdouble, GLdouble);
void (APIENTRY * glWindowPos4dvMESA) (const GLdouble *);
void (APIENTRY * glWindowPos4fMESA) (GLfloat, GLfloat, GLfloat, GLfloat);
void (APIENTRY * glWindowPos4fvMESA) (const GLfloat *);
void (APIENTRY * glWindowPos4iMESA) (GLint, GLint, GLint, GLint);
void (APIENTRY * glWindowPos4ivMESA) (const GLint *);
void (APIENTRY * glWindowPos4sMESA) (GLshort, GLshort, GLshort, GLshort);
void (APIENTRY * glWindowPos4svMESA) (const GLshort *);

void (APIENTRY * glMultiModeDrawArraysIBM) (GLenum, const GLint *, const GLsizei *, GLsizei, GLint);
void (APIENTRY * glMultiModeDrawElementsIBM) (const GLenum *, const GLsizei *, GLenum, const GLvoid* *, GLsizei, GLint);

void (APIENTRY * glColorPointerListIBM) (GLint, GLenum, GLint, const GLvoid* *, GLint);
void (APIENTRY * glSecondaryColorPointerListIBM) (GLint, GLenum, GLint, const GLvoid* *, GLint);
void (APIENTRY * glEdgeFlagPointerListIBM) (GLint, const GLboolean* *, GLint);
void (APIENTRY * glFogCoordPointerListIBM) (GLenum, GLint, const GLvoid* *, GLint);
void (APIENTRY * glIndexPointerListIBM) (GLenum, GLint, const GLvoid* *, GLint);
void (APIENTRY * glNormalPointerListIBM) (GLenum, GLint, const GLvoid* *, GLint);
void (APIENTRY * glTexCoordPointerListIBM) (GLint, GLenum, GLint, const GLvoid* *, GLint);
void (APIENTRY * glVertexPointerListIBM) (GLint, GLenum, GLint, const GLvoid* *, GLint);

void (APIENTRY * glTbufferMask3DFX) (GLuint);

void (APIENTRY * glSampleMaskEXT) (GLclampf, GLboolean);
void (APIENTRY * glSamplePatternEXT) (GLenum);
void (APIENTRY * glTextureColorMaskSGIS) (GLboolean, GLboolean, GLboolean, GLboolean);

char (APIENTRY * wglGetExtensionsStringARB) (HDC);
*/

/* NV_spec */
/*
void (APIENTRY * glGenFencesNV) (GLsizei n, GLuint *fences);
void (APIENTRY * glDeleteFencesNV) (GLsizei n, const GLuint *fences);
void (APIENTRY * glSetFenceNV) (GLuint fence, GLenum condition);
GLboolean (APIENTRY * glTestFenceNV) (GLuint fence);
void (APIENTRY * glFinishFenceNV) (GLuint fence);
void (APIENTRY * glIsFenceNV) (GLuint fence);
void (APIENTRY * glGetFenceivNV) (GLuint fence, GLenum pname, GLint *params);
*/

/* NV_register_combiners2 */
//void * (APIENTRY * wglAllocateMemoryNV) (int size, float readfreq, float writefreq, float priority);
//void (APIENTRY * wglFreeMemoryNV) (void *pointer);

/* Ati bump-mapping */
/*
void (APIENTRY *glTexBumpParameterivATI)(GLenum pname, GLint *param);
void (APIENTRY *glTexBumpParameterfvATI)(GLenum pname, GLfloat *param);
void (APIENTRY *glGetTexBumpParameterivATI)(GLenum pname, GLint *param);
void (APIENTRY *glGetTexBumpParameterfvATI)(GLenum pname, GLfloat *param);
*/
/*
PFNGLNEWOBJECTBUFFERATIPROC       glNewObjectBufferATI = NULL;
PFNGLISOBJECTBUFFERATIPROC        glIsObjectBufferATI = NULL;
PFNGLUPDATEOBJECTBUFFERATIPROC    glUpdateObjectBufferATI = NULL;
PFNGLGETOBJECTBUFFERFVATIPROC     glGetObjectBufferfvATI = NULL;
PFNGLGETOBJECTBUFFERIVATIPROC     glGetObjectBufferivATI = NULL;
PFNGLFREEOBJECTBUFFERATIPROC      glFreeObjectBufferATI = NULL;
PFNGLARRAYOBJECTATIPROC           glArrayObjectATI = NULL;
PFNGLGETARRAYOBJECTFVATIPROC      glGetArrayObjectfvATI = NULL;
PFNGLGETARRAYOBJECTIVATIPROC      glGetArrayObjectivATI = NULL;
PFNGLVARIANTARRAYOBJECTATIPROC    glVariantArrayObjectATI = NULL;
PFNGLGETVARIANTARRAYOBJECTFVATIPROC glGetVariantArrayObjectfvATI = NULL;
PFNGLGETVARIANTARRAYOBJECTIVATIPROC glGetVariantArrayObjectivATI = NULL;

PFNGLELEMENTPOINTERATIPROC        glElementPointerATI = NULL;
PFNGLDRAWELEMENTARRAYATIPROC      glDrawElementArrayATI = NULL;
PFNGLDRAWRANGEELEMENTARRAYATIPROC glDrawRangeElementArrayATI = NULL;
*/
/* ARB Vertex buffer
*/
/*
PFNGLBINDBUFFERARBPROC            glBindBufferARB = NULL;
PFNGLDELETEBUFFERSARBPROC         glDeleteBuffersARB = NULL;
PFNGLGENBUFFERSARBPROC            glGenBuffersARB = NULL;
PFNGLISBUFFERARBPROC              glIsBufferARB = NULL;
PFNGLBUFFERDATAARBPROC            glBufferDataARB = NULL;
PFNGLBUFFERSUBDATAARBPROC         glBufferSubDataARB = NULL;
PFNGLGETBUFFERSUBDATAARBPROC      glGetBufferSubDataARB = NULL;
PFNGLMAPBUFFERARBPROC             glMapBufferARB = NULL;
PFNGLUNMAPBUFFERARBPROC           glUnmapBufferARB = NULL;
PFNGLGETBUFFERPARAMETERIVARBPROC  glGetBufferParameterivARB = NULL;
PFNGLGETBUFFERPOINTERVARBPROC     glGetBufferPointervARB = NULL;
*/

/* ATI_pn_triangles */
/*
PFNGLPNTRIANGLESIATIPROC          glPNTrianglesiATI = NULL;
PFNGLPNTRIANGLESFATIPROC          glPNTrianglesfATI = NULL;
*/
/*
int gl_load_extension(HW_KONFIG *p_hwconf)
{
  const byte *p_vend = glGetString(GL_VENDOR);
  const byte *p_rend = glGetString(GL_RENDERER);
  const byte *p_ver = glGetString(GL_VERSION);
  const byte *p_et = glGetString(GL_EXTENSIONS);
  const byte *p_start;
  byte  *p_ext;
  byte  *p_pom;
  int   i,j;

  p_ext = mmalloc(sizeof(byte)*(strlen(p_et)+1));
  strcpy(p_ext,p_et);

  memset(&extlist,0,sizeof(extlist));

  extlist.p_vend = p_vend; // firma (ATI/NVIDIA/MATROX/....)
  extlist.p_rend = p_rend; // karta (TNT/GEFORCE/RADEON....)
  extlist.p_ver = p_ver;   // verze OpenGl

  kprintf(1,"Vendor: %s",p_vend);
  kprintf(1,"Renderer: %s",p_rend);
  kprintf(1,"Verze: %s",p_ver);  
  kprintf(1,"OpenGL Extensions:");

  j = strlen(p_ext);
  p_hwconf->extnum = 0;
  for(i = 0; i < j; i++) {
    if(p_ext[i] == ' ')
      p_hwconf->extnum++;
  }

  p_hwconf->p_ext = mmalloc(sizeof(byte *)*p_hwconf->extnum);


  i = 0;
  p_start = p_ext;
  while((p_pom = strchr(p_start,' '))) {
    *p_pom = 0;
    assert(i < p_hwconf->extnum);
    p_hwconf->p_ext[i] = mmalloc(sizeof(byte)*(strlen(p_start)+5));
      
    strcpy(p_hwconf->p_ext[i],p_start);
    i++;
    kprintf(1,"%s",p_start);
    p_start = p_pom+1;
  }

  free(p_ext);
  return(i);
}

int gl_free_extension(HW_KONFIG *p_hwconf)
{
  int i;
  for(i = 0; i < p_hwconf->extnum; i++)
    null_free(p_hwconf->p_ext+i);
  null_free((void **)&p_hwconf->p_ext);
  return(i);
}


int gl_is_extension(HW_KONFIG *p_conf, byte *p_text)
{
  int i;
  for(i = 0; i < p_conf->extnum; i++) {
    if(!strcmp(p_text,p_conf->p_ext[i])) {
      kprintf(TRUE,"Using extension: %s",p_text);
      return(TRUE);
    }
  }  
  return(FALSE);
}

int gl_init_multitexture(HW_KONFIG *p_conf)
{
  int level = 0;

  if(gl_is_extension(p_conf, "GL_ARB_multitexture")) {

    glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &glstav_multitext_units);
    kprintf(1,"ARB Texture units %d",glstav_multitext_units);
     
    if(!(glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB")))
      return(FALSE);
    if(!(glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)wglGetProcAddress("glClientActiveTextureARB")))
      return(FALSE);
    if(!(glMultiTexCoord1dARB = (PFNGLMULTITEXCOORD1DARBPROC)wglGetProcAddress("glMultiTexCoord1dARB")))    
      return(level);
    if(!(glMultiTexCoord1dvARB = (PFNGLMULTITEXCOORD1DVARBPROC)wglGetProcAddress("glMultiTexCoord1dvARB")))
      return(level);
    if(!(glMultiTexCoord1fARB = (PFNGLMULTITEXCOORD1FARBPROC)wglGetProcAddress("glMultiTexCoord1fARB")))
      return(level);
    if(!(glMultiTexCoord1fvARB = (PFNGLMULTITEXCOORD1FVARBPROC)wglGetProcAddress("glMultiTexCoord1fvARB")))
      return(level);
    if(!(glMultiTexCoord1iARB = (PFNGLMULTITEXCOORD1IARBPROC)wglGetProcAddress("glMultiTexCoord1iARB")))
      return(level);
    if(!(glMultiTexCoord1ivARB = (PFNGLMULTITEXCOORD1IVARBPROC)wglGetProcAddress("glMultiTexCoord1ivARB")))
      return(level);
    if(!(glMultiTexCoord1sARB = (PFNGLMULTITEXCOORD1SARBPROC)wglGetProcAddress("glMultiTexCoord1sARB")))
      return(level);
    if(!(glMultiTexCoord1svARB = (PFNGLMULTITEXCOORD1SVARBPROC)wglGetProcAddress("glMultiTexCoord1svARB")))
      return(level);
 
    level = 1;
    if(!(glMultiTexCoord2dARB = (PFNGLMULTITEXCOORD2DARBPROC)wglGetProcAddress("glMultiTexCoord2dARB")))
      return(level);
    if(!(glMultiTexCoord2dvARB = (PFNGLMULTITEXCOORD2DVARBPROC)wglGetProcAddress("glMultiTexCoord2dvARB")))
      return(level);
    if(!(glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress("glMultiTexCoord2fARB")))
      return(level);
    if(!(glMultiTexCoord2fvARB = (PFNGLMULTITEXCOORD2FVARBPROC)wglGetProcAddress("glMultiTexCoord2fvARB")))
      return(level);
    if(!(glMultiTexCoord2iARB = (PFNGLMULTITEXCOORD2IARBPROC)wglGetProcAddress("glMultiTexCoord2iARB")))
      return(level);
    if(!(glMultiTexCoord2ivARB = (PFNGLMULTITEXCOORD2IVARBPROC)wglGetProcAddress("glMultiTexCoord2ivARB")))
      return(level);
    if(!(glMultiTexCoord2sARB = (PFNGLMULTITEXCOORD2SARBPROC)wglGetProcAddress("glMultiTexCoord2sARB")))
      return(level);
    if(!(glMultiTexCoord2svARB = (PFNGLMULTITEXCOORD2SVARBPROC)wglGetProcAddress("glMultiTexCoord2svARB")))
      return(level);
    
    level = 2;
    if(!(glMultiTexCoord3dARB = (PFNGLMULTITEXCOORD3DARBPROC)wglGetProcAddress("glMultiTexCoord3dARB")))
      return(level);
    if(!(glMultiTexCoord3dvARB = (PFNGLMULTITEXCOORD3DVARBPROC)wglGetProcAddress("glMultiTexCoord3dvARB")))
      return(level);
    if(!(glMultiTexCoord3fARB = (PFNGLMULTITEXCOORD3FARBPROC)wglGetProcAddress("glMultiTexCoord3fARB")))
      return(level);
    if(!(glMultiTexCoord3fvARB = (PFNGLMULTITEXCOORD3FVARBPROC)wglGetProcAddress("glMultiTexCoord3fvARB")))
      return(level);
    if(!(glMultiTexCoord3iARB = (PFNGLMULTITEXCOORD3IARBPROC)wglGetProcAddress("glMultiTexCoord3iARB")))
      return(level);
    if(!(glMultiTexCoord3ivARB = (PFNGLMULTITEXCOORD3IVARBPROC)wglGetProcAddress("glMultiTexCoord3ivARB")))
      return(level);
    if(!(glMultiTexCoord3sARB = (PFNGLMULTITEXCOORD3SARBPROC)wglGetProcAddress("glMultiTexCoord3sARB")))
      return(level);
    if(!(glMultiTexCoord3svARB = (PFNGLMULTITEXCOORD3SVARBPROC)wglGetProcAddress("glMultiTexCoord3svARB")))
      return(level);
    
    level = 3;
    if(!(glMultiTexCoord4dARB = (PFNGLMULTITEXCOORD4DARBPROC)wglGetProcAddress("glMultiTexCoord4dARB")))
      return(level);
    if(!(glMultiTexCoord4dvARB = (PFNGLMULTITEXCOORD4DVARBPROC)wglGetProcAddress("glMultiTexCoord4dvARB")))
      return(level);
    if(!(glMultiTexCoord4fARB = (PFNGLMULTITEXCOORD4FARBPROC)wglGetProcAddress("glMultiTexCoord4fARB")))
      return(level);
    if(!(glMultiTexCoord4fvARB = (PFNGLMULTITEXCOORD4FVARBPROC)wglGetProcAddress("glMultiTexCoord4fvARB")))
      return(level);
    if(!(glMultiTexCoord4iARB = (PFNGLMULTITEXCOORD4IARBPROC)wglGetProcAddress("glMultiTexCoord4iARB")))
      return(level);
    if(!(glMultiTexCoord4ivARB = (PFNGLMULTITEXCOORD4IVARBPROC)wglGetProcAddress("glMultiTexCoord4ivARB")))
      return(level);
    if(!(glMultiTexCoord4sARB = (PFNGLMULTITEXCOORD4SARBPROC)wglGetProcAddress("glMultiTexCoord4sARB")))
      return(level);
    if(!(glMultiTexCoord4svARB = (PFNGLMULTITEXCOORD4SVARBPROC)wglGetProcAddress("glMultiTexCoord4svARB")))
      return(level);
    
    level = 4;
    return(level);
  } else {
    return(FALSE);
  }
}

int gl_init_blend_color(HW_KONFIG *p_conf)
{  
  if(gl_is_extension(p_conf, "GL_EXT_blend_color")) {
    if(!(glBlendColorEXT = (PFNGLBLENDCOLOREXTPROC)wglGetProcAddress("glBlendColorEXT")))
      return(FALSE);
    else
      return(TRUE);
  } else {
    return(FALSE);
  }
}

int gl_init_blend_minmax(HW_KONFIG *p_conf)
{  
  if(gl_is_extension(p_conf, "GL_EXT_blend_minmax")) {    
    if(!(glBlendEquationEXT = (PFNGLBLENDEQUATIONEXTPROC)wglGetProcAddress("glBlendEquationEXT")))
      return(FALSE);
    return(TRUE);
  } else {
    return(FALSE);
  }
}

int gl_init_fog_coord(HW_KONFIG *p_conf)
{  
  if(gl_is_extension(p_conf, "GL_EXT_fog_coord")) {
    if(!(glFogCoorddEXT = (PFNGLFOGCOORDDEXTPROC)wglGetProcAddress("glFogCoorddEXT")))
      return(FALSE);
    if(!(glFogCoorddvEXT = (PFNGLFOGCOORDDVEXTPROC)wglGetProcAddress("glFogCoorddvEXT")))
      return(FALSE);
    if(!(glFogCoordfEXT = (PFNGLFOGCOORDFEXTPROC)wglGetProcAddress("glFogCoordfEXT")))
      return(FALSE);
    if(!(glFogCoordfvEXT = (PFNGLFOGCOORDFVEXTPROC)wglGetProcAddress("glFogCoordfvEXT")))
      return(FALSE);
    if(!(glFogCoordPointerEXT = (PFNGLFOGCOORDPOINTEREXTPROC)wglGetProcAddress("glFogCoordPointerEXT")))
      return(FALSE);
            
    glFogi(GL_FOG_COORDINATE_SOURCE_EXT, GL_FRAGMENT_DEPTH_EXT);
    glDisableClientState(GL_FOG_COORDINATE_ARRAY_EXT);
   
    return(TRUE);
  } else {
    return(FALSE);
  }
}

int gl_init_secondary_color(HW_KONFIG *p_conf)
{  
  if(gl_is_extension(p_conf, "GL_EXT_secondary_color")) {
    if(!(glSecondaryColor3bEXT = (PFNGLSECONDARYCOLOR3BEXTPROC)wglGetProcAddress("glSecondaryColor3bEXT")))
      return(FALSE);
    if(!(glSecondaryColor3bvEXT = (PFNGLSECONDARYCOLOR3BVEXTPROC)wglGetProcAddress("glSecondaryColor3bvEXT")))
      return(FALSE);
    if(!(glSecondaryColor3dEXT = (PFNGLSECONDARYCOLOR3DEXTPROC)wglGetProcAddress("glSecondaryColor3dEXT")))
      return(FALSE);
    if(!(glSecondaryColor3dvEXT = (PFNGLSECONDARYCOLOR3DVEXTPROC)wglGetProcAddress("glSecondaryColor3dvEXT")))
      return(FALSE);
    if(!(glSecondaryColor3fEXT = (PFNGLSECONDARYCOLOR3FEXTPROC)wglGetProcAddress("glSecondaryColor3fEXT")))
      return(FALSE);
    if(!(glSecondaryColor3fvEXT = (PFNGLSECONDARYCOLOR3FVEXTPROC)wglGetProcAddress("glSecondaryColor3fvEXT")))
      return(FALSE);
    if(!(glSecondaryColor3iEXT = (PFNGLSECONDARYCOLOR3IEXTPROC)wglGetProcAddress("glSecondaryColor3iEXT")))
      return(FALSE);
    if(!(glSecondaryColor3ivEXT = (PFNGLSECONDARYCOLOR3IVEXTPROC)wglGetProcAddress("glSecondaryColor3ivEXT")))
      return(FALSE);
    if(!(glSecondaryColor3sEXT = (PFNGLSECONDARYCOLOR3SEXTPROC)wglGetProcAddress("glSecondaryColor3sEXT")))
      return(FALSE);
    if(!(glSecondaryColor3svEXT = (PFNGLSECONDARYCOLOR3SVEXTPROC)wglGetProcAddress("glSecondaryColor3svEXT")))
      return(FALSE);
    if(!(glSecondaryColor3ubEXT = (PFNGLSECONDARYCOLOR3UBEXTPROC)wglGetProcAddress("glSecondaryColor3ubEXT")))
      return(FALSE);
    if(!(glSecondaryColor3ubvEXT = (PFNGLSECONDARYCOLOR3UBVEXTPROC)wglGetProcAddress("glSecondaryColor3ubvEXT")))
      return(FALSE);
    if(!(glSecondaryColor3uiEXT = (PFNGLSECONDARYCOLOR3UIEXTPROC)wglGetProcAddress("glSecondaryColor3uiEXT")))
      return(FALSE);
    if(!(glSecondaryColor3uivEXT = (PFNGLSECONDARYCOLOR3UIVEXTPROC)wglGetProcAddress("glSecondaryColor3uivEXT")))
      return(FALSE);
    if(!(glSecondaryColor3usEXT = (PFNGLSECONDARYCOLOR3USEXTPROC)wglGetProcAddress("glSecondaryColor3usEXT")))
      return(FALSE);
    if(!(glSecondaryColor3usvEXT = (PFNGLSECONDARYCOLOR3USVEXTPROC)wglGetProcAddress("glSecondaryColor3usvEXT")))
      return(FALSE);
    if(!(glSecondaryColorPointerEXT = (PFNGLSECONDARYCOLORPOINTEREXTPROC)wglGetProcAddress("glSecondaryColorPointerEXT")))
      return(FALSE);
    return(TRUE);
  } else {
    return(FALSE);
  }
}

int gl_init_fence(HW_KONFIG *p_conf)
{  
  if(gl_is_extension(p_conf, "GL_NV_fence")) {
    if(!(glGenFencesNV = (PFNGLGENFENCESNVPROC)wglGetProcAddress("glGenFencesNV")))
      return(FALSE);
    if(!(glDeleteFencesNV = (PFNGLDELETEFENCESNVPROC)wglGetProcAddress("glDeleteFencesNV")))
      return(FALSE);
    if(!(glSetFenceNV = (PFNGLSETFENCENVPROC)wglGetProcAddress("glSetFenceNV")))
      return(FALSE);
    if(!(glTestFenceNV = (PFNGLTESTFENCENVPROC)wglGetProcAddress("glTestFenceNV")))
      return(FALSE);
    if(!(glFinishFenceNV = (PFNGLFINISHFENCENVPROC)wglGetProcAddress("glFinishFenceNV")))
      return(FALSE);
    if(!(glIsFenceNV = (PFNGLISFENCENVPROC)wglGetProcAddress("glIsFenceNV")))
      return(FALSE);
    if(!(glGetFenceivNV = (PFNGLGETFENCEIVNVPROC)wglGetProcAddress("glGetFenceivNV")))
      return(FALSE);
    return(TRUE);
  } else {
    return(FALSE);
  }
}

int gl_init_texture_compression(HW_KONFIG *p_conf)
{  
  if(gl_is_extension(p_conf, "GL_ARB_texture_compression")) {
    return(TRUE);
  } else {
    return(FALSE);
  }
}

int gl_init_texture_lod(HW_KONFIG *p_conf)
{  
  if(gl_is_extension(p_conf, "GL_EXT_texture_lod_bias")) {
    return(TRUE);
  } else {
    return(FALSE);
  }
}

int gl_init_clip_volume_hit(HW_KONFIG *p_conf)
{  
  if(gl_is_extension(p_conf, "GL_EXT_clip_volume_hint")) {
    return(TRUE);
  } else {
    return(FALSE);
  }
}

int gl_init_text_edge_clamp(HW_KONFIG *p_conf)
{  
  if(gl_is_extension(p_conf, "GL_EXT_texture_edge_clamp")) {
    return(TRUE);
  } else {
    return(FALSE);
  }
}

int gl_init_text_env_combine(HW_KONFIG *p_conf)
{  
  if(gl_is_extension(p_conf, "GL_EXT_texture_env_combine")) {
    return(TRUE);
  } else {
    return(FALSE);
  }
}

int gl_init_text_env_dot3(HW_KONFIG *p_conf)
{  
  if(gl_is_extension(p_conf, "GL_EXT_texture_env_dot3")) {
    return(TRUE);
  } else {
    return(FALSE);
  }
}

int gl_init_ati_env_bump(HW_KONFIG *p_conf)
{  
  int *p_units,units;
  int i,u;  

  if(gl_is_extension(p_conf, "GL_ATI_envmap_bumpmap")) {
    glTexBumpParameterivATI = (PFNGLTEXBUMPPARAMETERIVATIPROC) wglGetProcAddress("glTexBumpParameterivATI");
    glTexBumpParameterfvATI = (PFNGLTEXBUMPPARAMETERFVATIPROC) wglGetProcAddress("glTexBumpParameterfvATI");
    glGetTexBumpParameterivATI = (PFNGLGETTEXBUMPPARAMETERIVATIPROC) wglGetProcAddress("glGetTexBumpParameterivATI");
    glGetTexBumpParameterfvATI = (PFNGLGETTEXBUMPPARAMETERFVATIPROC) wglGetProcAddress("glGetTexBumpParameterfvATI");
    
    glGetTexBumpParameterivATI(GL_BUMP_NUM_TEX_UNITS_ATI,&units);
    kprintf(1,"ATI Bump-units %d",units);
    p_units = _alloca(sizeof(p_units[0])*units);    
    glGetTexBumpParameterivATI(GL_BUMP_TEX_UNITS_ATI,p_units);

    for(i = 0; i < units; i++) {
      u = p_units[i]-GL_TEXTURE0_ARB;
      extlist_ati_env_bump_unit[u] = TRUE;
      kprintf(1,"Bump unit %d",u);
    }
    return(TRUE);
  } else {
    return(FALSE);
  }
}

int gl_init_vertex_array_object(HW_KONFIG *p_conf)
{  
  if(gl_is_extension(p_conf, "GL_ATI_vertex_array_object")) {
    glNewObjectBufferATI = (PFNGLNEWOBJECTBUFFERATIPROC) wglGetProcAddress("glNewObjectBufferATI");
    glIsObjectBufferATI = (PFNGLISOBJECTBUFFERATIPROC) wglGetProcAddress("glIsObjectBufferATI");
    glUpdateObjectBufferATI = (PFNGLUPDATEOBJECTBUFFERATIPROC) wglGetProcAddress("glUpdateObjectBufferATI");
    glGetObjectBufferfvATI = (PFNGLGETOBJECTBUFFERFVATIPROC) wglGetProcAddress("glGetObjectBufferfvATI");
    glGetObjectBufferivATI = (PFNGLGETOBJECTBUFFERIVATIPROC) wglGetProcAddress("glGetObjectBufferivATI");
    glFreeObjectBufferATI = (PFNGLFREEOBJECTBUFFERATIPROC) wglGetProcAddress("glFreeObjectBufferATI");
    glArrayObjectATI = (PFNGLARRAYOBJECTATIPROC) wglGetProcAddress("glArrayObjectATI");
    glGetArrayObjectfvATI = (PFNGLGETARRAYOBJECTFVATIPROC) wglGetProcAddress("glGetArrayObjectfvATI");
    glGetArrayObjectivATI = (PFNGLGETARRAYOBJECTIVATIPROC) wglGetProcAddress("glGetArrayObjectivATI");
    glVariantArrayObjectATI = (PFNGLVARIANTARRAYOBJECTATIPROC) wglGetProcAddress("glVariantArrayObjectATI");
    glGetVariantArrayObjectfvATI = (PFNGLGETVARIANTARRAYOBJECTFVATIPROC) wglGetProcAddress("glGetVariantArrayObjectfvATI");
    glGetVariantArrayObjectivATI = (PFNGLGETVARIANTARRAYOBJECTIVATIPROC) wglGetProcAddress("glGetVariantArrayObjectivATI");
    return(TRUE);
  } else {
    return(FALSE);
  }
}

int gl_init_element_array(HW_KONFIG *p_conf)
{  
  if(gl_is_extension(p_conf, "GL_ATI_element_array")) {
    glElementPointerATI = (PFNGLELEMENTPOINTERATIPROC) wglGetProcAddress("glElementPointerATI");
    glDrawElementArrayATI = (PFNGLDRAWELEMENTARRAYATIPROC) wglGetProcAddress("glDrawElementArrayATI");
    glDrawRangeElementArrayATI = (PFNGLDRAWRANGEELEMENTARRAYATIPROC) wglGetProcAddress("glDrawRangeElementArrayATI");
    return(TRUE);
  } else {
    return(FALSE);
  }
}

int gl_init_register_combiners(HW_KONFIG *p_conf)
{  
  if(gl_is_extension(p_conf, "GL_NV_register_combiners")) {    
    if(!(glCombinerParameterfvNV = (PFNGLCOMBINERPARAMETERFVNVPROC)wglGetProcAddress("glCombinerParameterfvNV")))
      return(FALSE);
    if(!(glCombinerParameterfNV = (PFNGLCOMBINERPARAMETERFNVPROC)wglGetProcAddress("glCombinerParameterfNV")))
      return(FALSE);
    if(!(glCombinerParameterivNV = (PFNGLCOMBINERPARAMETERIVNVPROC)wglGetProcAddress("glCombinerParameterivNV")))
      return(FALSE);
    if(!(glCombinerParameteriNV = (PFNGLCOMBINERPARAMETERINVPROC)wglGetProcAddress("glCombinerParameteriNV")))
      return(FALSE);
    if(!(glCombinerInputNV = (PFNGLCOMBINERINPUTNVPROC)wglGetProcAddress("glCombinerInputNV")))
      return(FALSE);
    if(!(glCombinerOutputNV = (PFNGLCOMBINEROUTPUTNVPROC)wglGetProcAddress("glCombinerOutputNV")))
      return(FALSE);
    if(!(glFinalCombinerInputNV = (PFNGLFINALCOMBINERINPUTNVPROC)wglGetProcAddress("glFinalCombinerInputNV")))
      return(FALSE);
    if(!(glGetCombinerInputParameterfvNV = (PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC)wglGetProcAddress("glGetCombinerInputParameterfvNV")))
      return(FALSE);
    if(!(glGetCombinerInputParameterivNV = (PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC)wglGetProcAddress("glGetCombinerInputParameterivNV")))
      return(FALSE);
    if(!(glGetCombinerOutputParameterfvNV = (PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC)wglGetProcAddress("glGetCombinerOutputParameterfvNV")))
      return(FALSE);
    if(!(glGetCombinerOutputParameterivNV = (PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC)wglGetProcAddress("glGetCombinerOutputParameterivNV")))
      return(FALSE);
    if(!(glGetFinalCombinerInputParameterfvNV = (PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC)wglGetProcAddress("glGetFinalCombinerInputParameterfvNV")))
      return(FALSE);
    if(!(glGetFinalCombinerInputParameterivNV = (PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC)wglGetProcAddress("glGetFinalCombinerInputParameterivNV")))
      return(FALSE);
    return(TRUE);
  } else {
    return(FALSE);
  }
}


int gl_init_vertex_array_range(HW_KONFIG *p_conf)
{
  if(gl_is_extension(p_conf, "GL_NV_vertex_array_range")) {
    if(!(glFlushVertexArrayRangeNV = (PFNGLFLUSHVERTEXARRAYRANGENVPROC)wglGetProcAddress("glFlushVertexArrayRangeNV")))
      return(FALSE);
    if(!(glVertexArrayRangeNV = (PFNGLVERTEXARRAYRANGENVPROC)wglGetProcAddress("glVertexArrayRangeNV")))
      return(FALSE);
    if(!(wglAllocateMemoryNV = (PFNWGLALLOCATEMEMORYNVPROC)wglGetProcAddress("wglAllocateMemoryNV")))
      return(FALSE);
    if(!(wglFreeMemoryNV = (PFNWGLFREEMEMORYNVPROC)wglGetProcAddress("wglFreeMemoryNV")))
      return(FALSE);
    return(TRUE);
  } else {
    return(FALSE);
  }
}

int gl_init_vertex_array_packed(HW_KONFIG *p_conf)
{  
  if(gl_is_extension(p_conf, "GL_EXT_compiled_vertex_array")) {

    if(!(glLockArraysEXT = (PFNGLLOCKARRAYSEXTPROC)wglGetProcAddress("glLockArraysEXT")))
      return(FALSE);
    if(!(glUnlockArraysEXT = (PFNGLUNLOCKARRAYSEXTPROC)wglGetProcAddress("glUnlockArraysEXT")))
      return(FALSE);
    
    return(TRUE);
  } else {
    return(FALSE);
  }
}

int gl_init_ati_pn_triangles(HW_KONFIG *p_conf)
{  
  if(gl_is_extension(p_conf, "GL_ATI_pn_triangles")) {
    glPNTrianglesiATI = (PFNGLPNTRIANGLESIATIPROC) wglGetProcAddress("glPNTrianglesiATI");
    glPNTrianglesfATI = (PFNGLPNTRIANGLESFATIPROC) wglGetProcAddress("glPNTrianglesfATI");    
    return(TRUE);
  } else {
    return(FALSE);
  }
}

int gl_init_text_filter_anisotropic(HW_KONFIG *p_conf)
{  
  if(gl_is_extension(p_conf, "GL_EXT_texture_filter_anisotropic")) {
    return(TRUE);
  } else {
    return(FALSE);
  }
}

int gl_init_ati_draw_buffers(HW_KONFIG *p_conf)
{  
  if(gl_is_extension(p_conf, "GL_ATI_draw_buffers")) {
    return(TRUE);
  } else {
    return(FALSE);
  }
}

int gl_init_arb_vertex_buffer(HW_KONFIG *p_conf)
{  
  if(gl_is_extension(p_conf, "GL_ARB_vertex_buffer_object")) {
    if(!(glBindBufferARB = (PFNGLBINDBUFFERARBPROC)wglGetProcAddress("glBindBufferARB")))
      return(FALSE);
    if(!(glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)wglGetProcAddress("glDeleteBuffersARB")))
      return(FALSE);
    if(!(glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)wglGetProcAddress("glGenBuffersARB")))
      return(FALSE);
    if(!(glIsBufferARB = (PFNGLISBUFFERARBPROC)wglGetProcAddress("glIsBufferARB")))
      return(FALSE);
    if(!(glBufferDataARB = (PFNGLBUFFERDATAARBPROC)wglGetProcAddress("glBufferDataARB")))
      return(FALSE);
    if(!(glBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC)wglGetProcAddress("glBufferSubDataARB")))
      return(FALSE);
    if(!(glGetBufferSubDataARB = (PFNGLGETBUFFERSUBDATAARBPROC)wglGetProcAddress("glGetBufferSubDataARB")))
      return(FALSE);
    if(!(glMapBufferARB = (PFNGLMAPBUFFERARBPROC)wglGetProcAddress("glMapBufferARB")))
      return(FALSE);
    if(!(glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)wglGetProcAddress("glUnmapBufferARB")))
      return(FALSE);
    if(!(glGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC)wglGetProcAddress("glGetBufferParameterivARB")))
      return(FALSE);
    if(!(glGetBufferPointervARB = (PFNGLGETBUFFERPOINTERVARBPROC)wglGetProcAddress("glGetBufferPointervARB")))
      return(FALSE);
    return(TRUE);
  } else {
    return(FALSE);
  }
}


EXTENSION_FLAGS extlist;

int gl_init_extension(HW_KONFIG *p_hwconf)
{   
  kprintf(1,"Init extensions");
 
  extlist_multitexture = gl_init_multitexture(p_hwconf);
  extlist_blend_color = gl_init_blend_color(p_hwconf);
  extlist_blend_minmax = gl_init_blend_minmax(p_hwconf);
  extlist_fog_coord = gl_init_fog_coord(p_hwconf);
  extlist_secondary_color = gl_init_secondary_color(p_hwconf);
  extlist_fence = gl_init_fence(p_hwconf);
  extlist_register_combiners = gl_init_register_combiners(p_hwconf);
  extlist_text_compresion = gl_init_texture_compression(p_hwconf);
  extlist_text_sharp = gl_init_texture_lod(p_hwconf);
  extlist_text_edge_clamp = gl_init_text_edge_clamp(p_hwconf);
  extlist_text_env_combine = gl_init_text_env_combine(p_hwconf);
  extlist_text_filter_anisotropic = gl_init_text_filter_anisotropic(p_hwconf);
  extlist_clip_volume_hit = gl_init_clip_volume_hit(p_hwconf);
  extlist_vertex_array_range = gl_init_vertex_array_range(p_hwconf);
  extlist_vertex_array_packed = gl_init_vertex_array_packed(p_hwconf);
  extlist_ati_pn_triangles = gl_init_ati_pn_triangles(p_hwconf);
  extlist_ati_draw_buffers = gl_init_ati_draw_buffers(p_hwconf);  
*/
/*
  if(hwconf.vertex_arrays) {
    extlist_ati_vertex_array_object = gl_init_vertex_array_object(p_hwconf);
    extlist_ati_element_array = gl_init_element_array(p_hwconf);
    
    if(!(extlist_ati_vertex_array_object && extlist_ati_element_array)) {
       extlist_ati_vertex_array_object = 0;
       extlist_ati_element_array = 0;
       extlist_arb_vertex_buffer = gl_init_arb_vertex_buffer(p_hwconf);
    }
  }
*/
/*
  // pouze pokud je vic jak 2 texturovaci jednotky
  if(hwconf.bump_mapping && glstav_multitext_units > 2) {
    extlist_text_env_dot3 = gl_init_text_env_dot3(p_hwconf);
    extlist_ati_env_bump = gl_init_ati_env_bump(p_hwconf);
    hwconf.bump_mapping = extlist_text_env_dot3|extlist_ati_env_bump;
  }

  return(TRUE);
}
*/
/*
int gl_ext_default(void)
{
  glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)glDefaultFunkce;
  glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)glDefaultFunkce;
  glMultiTexCoord1dARB = (PFNGLMULTITEXCOORD1DARBPROC)glDefaultFunkce;
  glMultiTexCoord1dvARB = (PFNGLMULTITEXCOORD1DVARBPROC)glDefaultFunkce;
  glMultiTexCoord1fARB = (PFNGLMULTITEXCOORD1FARBPROC)glDefaultFunkce;
  glMultiTexCoord1fvARB = (PFNGLMULTITEXCOORD1FVARBPROC)glDefaultFunkce;
  glMultiTexCoord1iARB = (PFNGLMULTITEXCOORD1IARBPROC)glDefaultFunkce;
  glMultiTexCoord1ivARB = (PFNGLMULTITEXCOORD1IVARBPROC)glDefaultFunkce;
  glMultiTexCoord1sARB = (PFNGLMULTITEXCOORD1SARBPROC)glDefaultFunkce;
  glMultiTexCoord1svARB = (PFNGLMULTITEXCOORD1SVARBPROC)glDefaultFunkce;
  glMultiTexCoord2dARB = (PFNGLMULTITEXCOORD2DARBPROC)glDefaultFunkce;
  glMultiTexCoord2dvARB = (PFNGLMULTITEXCOORD2DVARBPROC)glDefaultFunkce;
  glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)glDefaultFunkce;
  glMultiTexCoord2fvARB = (PFNGLMULTITEXCOORD2FVARBPROC)glDefaultFunkce;
  glMultiTexCoord2iARB = (PFNGLMULTITEXCOORD2IARBPROC)glDefaultFunkce;
  glMultiTexCoord2ivARB = (PFNGLMULTITEXCOORD2IVARBPROC)glDefaultFunkce;
  glMultiTexCoord2sARB = (PFNGLMULTITEXCOORD2SARBPROC)glDefaultFunkce;
  glMultiTexCoord2svARB = (PFNGLMULTITEXCOORD2SVARBPROC)glDefaultFunkce;
  glMultiTexCoord3dARB = (PFNGLMULTITEXCOORD3DARBPROC)glDefaultFunkce;
  glMultiTexCoord3dvARB = (PFNGLMULTITEXCOORD3DVARBPROC)glDefaultFunkce;
  glMultiTexCoord3fARB = (PFNGLMULTITEXCOORD3FARBPROC)glDefaultFunkce;
  glMultiTexCoord3fvARB = (PFNGLMULTITEXCOORD3FVARBPROC)glDefaultFunkce;
  glMultiTexCoord3iARB = (PFNGLMULTITEXCOORD3IARBPROC)glDefaultFunkce;
  glMultiTexCoord3ivARB = (PFNGLMULTITEXCOORD3IVARBPROC)glDefaultFunkce;
  glMultiTexCoord3sARB = (PFNGLMULTITEXCOORD3SARBPROC)glDefaultFunkce;
  glMultiTexCoord3svARB = (PFNGLMULTITEXCOORD3SVARBPROC)glDefaultFunkce;
  glMultiTexCoord4dARB = (PFNGLMULTITEXCOORD4DARBPROC)glDefaultFunkce;
  glMultiTexCoord4dvARB = (PFNGLMULTITEXCOORD4DVARBPROC)glDefaultFunkce;
  glMultiTexCoord4fARB = (PFNGLMULTITEXCOORD4FARBPROC)glDefaultFunkce;
  glMultiTexCoord4fvARB = (PFNGLMULTITEXCOORD4FVARBPROC)glDefaultFunkce;
  glMultiTexCoord4iARB = (PFNGLMULTITEXCOORD4IARBPROC)glDefaultFunkce;
  glMultiTexCoord4ivARB = (PFNGLMULTITEXCOORD4IVARBPROC)glDefaultFunkce;
  glMultiTexCoord4sARB = (PFNGLMULTITEXCOORD4SARBPROC)glDefaultFunkce;
  glMultiTexCoord4svARB = (PFNGLMULTITEXCOORD4SVARBPROC)glDefaultFunkce;
  glBlendColorEXT = (PFNGLBLENDCOLOREXTPROC)glDefaultFunkce;
  glBlendEquationEXT = (PFNGLBLENDEQUATIONEXTPROC)glDefaultFunkce;
  glFogCoorddEXT = (PFNGLFOGCOORDDEXTPROC)glDefaultFunkce;
  glFogCoorddvEXT = (PFNGLFOGCOORDDVEXTPROC)glDefaultFunkce;
  glFogCoordfEXT = (PFNGLFOGCOORDFEXTPROC)glDefaultFunkce;
  glFogCoordfvEXT = (PFNGLFOGCOORDFVEXTPROC)glDefaultFunkce;
  glFogCoordPointerEXT = (PFNGLFOGCOORDPOINTEREXTPROC)glDefaultFunkce;
  glSecondaryColor3bEXT = (PFNGLSECONDARYCOLOR3BEXTPROC)glDefaultFunkce;
  glSecondaryColor3bvEXT = (PFNGLSECONDARYCOLOR3BVEXTPROC)glDefaultFunkce;
  glSecondaryColor3dEXT = (PFNGLSECONDARYCOLOR3DEXTPROC)glDefaultFunkce;
  glSecondaryColor3dvEXT = (PFNGLSECONDARYCOLOR3DVEXTPROC)glDefaultFunkce;
  glSecondaryColor3fEXT = (PFNGLSECONDARYCOLOR3FEXTPROC)glDefaultFunkce;
  glSecondaryColor3fvEXT = (PFNGLSECONDARYCOLOR3FVEXTPROC)glDefaultFunkce;
  glSecondaryColor3iEXT = (PFNGLSECONDARYCOLOR3IEXTPROC)glDefaultFunkce;
  glSecondaryColor3ivEXT = (PFNGLSECONDARYCOLOR3IVEXTPROC)glDefaultFunkce;
  glSecondaryColor3sEXT = (PFNGLSECONDARYCOLOR3SEXTPROC)glDefaultFunkce;
  glSecondaryColor3svEXT = (PFNGLSECONDARYCOLOR3SVEXTPROC)glDefaultFunkce;
  glSecondaryColor3ubEXT = (PFNGLSECONDARYCOLOR3UBEXTPROC)glDefaultFunkce;
  glSecondaryColor3ubvEXT = (PFNGLSECONDARYCOLOR3UBVEXTPROC)glDefaultFunkce;
  glSecondaryColor3uiEXT = (PFNGLSECONDARYCOLOR3UIEXTPROC)glDefaultFunkce;
  glSecondaryColor3uivEXT = (PFNGLSECONDARYCOLOR3UIVEXTPROC)glDefaultFunkce;
  glSecondaryColor3usEXT = (PFNGLSECONDARYCOLOR3USEXTPROC)glDefaultFunkce;
  glSecondaryColor3usvEXT = (PFNGLSECONDARYCOLOR3USVEXTPROC)glDefaultFunkce;
  glSecondaryColorPointerEXT = (PFNGLSECONDARYCOLORPOINTEREXTPROC)glDefaultFunkce;
  glGenFencesNV = (PFNGLGENFENCESNVPROC)glDefaultFunkce;
  glDeleteFencesNV = (PFNGLDELETEFENCESNVPROC)glDefaultFunkce;
  glSetFenceNV = (PFNGLSETFENCENVPROC)glDefaultFunkce;
  glTestFenceNV = (PFNGLTESTFENCENVPROC)glDefaultFunkce;
  glFinishFenceNV = (PFNGLFINISHFENCENVPROC)glDefaultFunkce;
  glIsFenceNV = (PFNGLISFENCENVPROC)glDefaultFunkce;
  glGetFenceivNV = (PFNGLGETFENCEIVNVPROC)glDefaultFunkce;
  glCombinerParameterfvNV = (PFNGLCOMBINERPARAMETERFVNVPROC)glDefaultFunkce;
  glCombinerParameterfNV = (PFNGLCOMBINERPARAMETERFNVPROC)glDefaultFunkce;
  glCombinerParameterivNV = (PFNGLCOMBINERPARAMETERIVNVPROC)glDefaultFunkce;
  glCombinerParameteriNV = (PFNGLCOMBINERPARAMETERINVPROC)glDefaultFunkce;
  glCombinerInputNV = (PFNGLCOMBINERINPUTNVPROC)glDefaultFunkce;
  glCombinerOutputNV = (PFNGLCOMBINEROUTPUTNVPROC)glDefaultFunkce;
  glFinalCombinerInputNV = (PFNGLFINALCOMBINERINPUTNVPROC)glDefaultFunkce;
  glGetCombinerInputParameterfvNV = (PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC)glDefaultFunkce;
  glGetCombinerInputParameterivNV = (PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC)glDefaultFunkce;
  glGetCombinerOutputParameterfvNV = (PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC)glDefaultFunkce;
  glGetCombinerOutputParameterivNV = (PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC)glDefaultFunkce;
  glGetFinalCombinerInputParameterfvNV = (PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC)glDefaultFunkce;
  glGetFinalCombinerInputParameterivNV = (PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC)glDefaultFunkce;
  glFlushVertexArrayRangeNV = (PFNGLFLUSHVERTEXARRAYRANGENVPROC)glDefaultFunkce;
  glVertexArrayRangeNV = (PFNGLVERTEXARRAYRANGENVPROC)glDefaultFunkce;
  wglAllocateMemoryNV = (PFNWGLALLOCATEMEMORYNVPROC)glDefaultFunkce;
  wglFreeMemoryNV = (PFNWGLFREEMEMORYNVPROC)glDefaultFunkce;
  glArrayElementEXT = (PFNGLARRAYELEMENTEXTPROC)glDefaultFunkce;
  glColorPointerEXT = (PFNGLCOLORPOINTEREXTPROC)glDefaultFunkce;
  glDrawArraysEXT = (PFNGLDRAWARRAYSEXTPROC)glDefaultFunkce;
  glEdgeFlagPointerEXT = (PFNGLEDGEFLAGPOINTEREXTPROC)glDefaultFunkce;
  glGetPointervEXT = (PFNGLGETPOINTERVEXTPROC)glDefaultFunkce;
  glIndexPointerEXT = (PFNGLINDEXPOINTEREXTPROC)glDefaultFunkce;
  glNormalPointerEXT = (PFNGLNORMALPOINTEREXTPROC)glDefaultFunkce;
  glTexCoordPointerEXT = (PFNGLTEXCOORDPOINTEREXTPROC)glDefaultFunkce;
  glVertexPointerEXT = (PFNGLVERTEXPOINTEREXTPROC)glDefaultFunkce;
  glLockArraysEXT = (PFNGLLOCKARRAYSEXTPROC)glDefaultFunkce;
  glUnlockArraysEXT = (PFNGLUNLOCKARRAYSEXTPROC)glDefaultFunkce;
  return(FALSE);
}
*/
