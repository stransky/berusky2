/*
  Header k animacim
*/

// byte-align structures
#ifdef _MSC_VER
#	pragma pack( push, packing )
#	pragma pack( 1 )
#	define PACK_STRUCT
#elif defined( __GNUC__ )
#	define PACK_STRUCT	__attribute__((packed))
#else
#	error you must byte-align these structures with the appropriate compiler directives
#endif

// File header
struct MS3DHeader
{
  char m_ID[10];
  int m_version;
} PACK_STRUCT;

// Vertex information
struct MS3DVertex
{
  byte m_flags;
  float m_vertex[3];
  char m_boneID;
  byte m_refCount;
} PACK_STRUCT;

// Triangle information
struct MS3DTriangle
{
  word m_flags;
  word m_vertexIndices[3];
  float m_vertexNormals[3][3];
  float m_s[3], m_t[3];
  byte m_smoothingGroup;
  byte m_groupIndex;
} PACK_STRUCT;

// Material information
struct MS3DMaterial
{
  char m_name[32];
  float m_ambient[4];
  float m_diffuse[4];
  float m_specular[4];
  float m_emissive[4];
  float m_shininess;            // 0.0f - 128.0f
  float m_transparency;         // 0.0f - 1.0f
  byte m_mode;                  // 0, 1, 2 is unused now
  char m_texture[128];
  char m_alphamap[128];
} PACK_STRUCT;

//      Joint information
struct MS3DJoint
{
  byte m_flags;
  char m_name[32];
  char m_parentName[32];
  float m_rotation[3];
  float m_translation[3];
  word m_numRotationKeyframes;
  word m_numTranslationKeyframes;
} PACK_STRUCT;

// Keyframe data
struct MS3DKeyframe
{
  float m_time;
  float m_parameter[3];
} PACK_STRUCT;
