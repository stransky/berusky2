/*
  Rozsireni pro open gl od nvidia
*/
/*
#ifdef __cplusplus
extern "C" {            
#endif  
*/
/* __cplusplus */


#ifndef __GGLEXT_H
#define __GGLEXT_H

/* TODO

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
*/

/*
extern void (APIENTRY *glTexBumpParameterivATI)(GLenum pname, GLint *param);
extern void (APIENTRY *glTexBumpParameterfvATI)(GLenum pname, GLfloat *param);
extern void (APIENTRY *glGetTexBumpParameterivATI)(GLenum pname, GLint *param);
extern void (APIENTRY *glGetTexBumpParameterfvATI)(GLenum pname, GLfloat *param);

extern PFNGLNEWOBJECTBUFFERATIPROC glNewObjectBufferATI;
extern PFNGLISOBJECTBUFFERATIPROC glIsObjectBufferATI;
extern PFNGLUPDATEOBJECTBUFFERATIPROC glUpdateObjectBufferATI;
extern PFNGLGETOBJECTBUFFERFVATIPROC glGetObjectBufferfvATI;
extern PFNGLGETOBJECTBUFFERIVATIPROC glGetObjectBufferivATI;
extern PFNGLFREEOBJECTBUFFERATIPROC glFreeObjectBufferATI;
extern PFNGLARRAYOBJECTATIPROC glArrayObjectATI;
extern PFNGLGETARRAYOBJECTFVATIPROC glGetArrayObjectfvATI;
extern PFNGLGETARRAYOBJECTIVATIPROC glGetArrayObjectivATI;
extern PFNGLVARIANTARRAYOBJECTATIPROC glVariantArrayObjectATI;
extern PFNGLGETVARIANTARRAYOBJECTFVATIPROC glGetVariantArrayObjectfvATI;
extern PFNGLGETVARIANTARRAYOBJECTIVATIPROC glGetVariantArrayObjectivATI;

extern PFNGLELEMENTPOINTERATIPROC glElementPointerATI;
extern PFNGLDRAWELEMENTARRAYATIPROC glDrawElementArrayATI;
extern PFNGLDRAWRANGEELEMENTARRAYATIPROC glDrawRangeElementArrayATI;

extern PFNGLPNTRIANGLESIATIPROC glPNTrianglesiATI;
extern PFNGLPNTRIANGLESFATIPROC glPNTrianglesfATI;

extern PFNGLBINDBUFFERARBPROC glBindBufferARB;
extern PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB;
extern PFNGLGENBUFFERSARBPROC glGenBuffersARB;
extern PFNGLISBUFFERARBPROC glIsBufferARB;
extern PFNGLBUFFERDATAARBPROC glBufferDataARB;
extern PFNGLBUFFERSUBDATAARBPROC glBufferSubDataARB;
extern PFNGLGETBUFFERSUBDATAARBPROC glGetBufferSubDataARB;
extern PFNGLMAPBUFFERARBPROC glMapBufferARB;
extern PFNGLUNMAPBUFFERARBPROC glUnmapBufferARB;
extern PFNGLGETBUFFERPARAMETERIVARBPROC glGetBufferParameterivARB;
extern PFNGLGETBUFFERPOINTERVARBPROC glGetBufferPointervARB;
*/

#define KARTA_DEFAULT  0
#define KARTA_ATI      1
#define KARTA_NVIDIA   2

typedef struct _EXTENSION_FLAGS {

  const char *p_vend;
  const char *p_rend;
  const char *p_ver;
  int   karta;

} EXTENSION_FLAGS;

extern EXTENSION_FLAGS extlist;

extern int extlist_multitexture;
extern int extlist_blend_color;
extern int extlist_blend_minmax;
extern int extlist_fog_coord;
extern int extlist_secondary_color;
extern int extlist_fence;
extern int extlist_register_combiners;
extern int extlist_text_compresion;
extern int extlist_text_sharp;
extern int extlist_text_edge_clamp;
extern int extlist_vertex_array_range;
extern int extlist_vertex_array;
extern int extlist_indicie_array;
extern int extlist_vertex_array_packed;
extern int extlist_text_env_combine;
extern int extlist_text_env_dot3;
extern int extlist_text_filter_anisotropic;
extern int extlist_clip_volume_hit;
extern int extlist_ati_env_bump;
extern int extlist_ati_env_bump_unit[6];
extern int extlist_ati_vertex_array_object;
extern int extlist_ati_element_array;
extern int extlist_ati_pn_triangles;
extern int extlist_arb_vertex_buffer;
extern int  extlist_ati_draw_buffers;


int gl_load_extension(HW_KONFIG *p_hwconf);
int gl_init_extension(HW_KONFIG *p_hwconf);
int gl_free_extension(HW_KONFIG *p_hwconf);
int gl_is_extension(HW_KONFIG *p_conf, char *p_text);
int gl_init_multitexture(HW_KONFIG *p_conf);
int gl_init_blend_color(HW_KONFIG *p_conf);
int gl_init_blend_minmax(HW_KONFIG *p_conf);
int gl_init_fog_coord(HW_KONFIG *p_conf);
int gl_init_secondary_color(HW_KONFIG *p_conf);
int gl_init_fence(HW_KONFIG *p_conf);
int gl_init_register_combiners(HW_KONFIG *p_conf);
int gl_init_vertex_array_range(HW_KONFIG *p_conf);
int gl_init_texture_compression(HW_KONFIG *p_conf);
int gl_init_text_edge_clamp(HW_KONFIG *p_conf);
int gl_init_clip_volume_hit(HW_KONFIG *p_conf);
int gl_init_ati_pn_triangles(HW_KONFIG *p_conf);
int gl_init_text_filter_anisotropic(HW_KONFIG *p_conf);
int gl_init_ati_draw_buffers(HW_KONFIG *p_conf);
int gl_ext_default(void);


#endif

/*
#ifdef __cplusplus
}
#endif
*/
