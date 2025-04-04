
#ifndef __HTYPES_H
#define __HTYPES_H

/**
	Common Layer���� ����ϴ� ���� Ÿ�� ����

	@name ����Ÿ�� ����
 */
//@{

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#undef NULL
#define NULL 0

#ifndef PUBLIC
#define PUBLIC
#endif

#if defined(__GNUC__ ) 
   #define BEGIN_PACKED_STRUCT( ST ) \
        typedef struct _##ST {
    #define END_PACKED_STRUCT  \
        } __attributed((packed)) ;
#elif defined(__CC_ARM)
   #define BEGIN_PACKED_STRUCT( ST ) \
        typedef __packed struct   _##ST {
    #define END_PACKED_STRUCT(x)  \
        }  x;
#elif defined(_WIN32)
#pragma pack(1) 
	#define BEGIN_PACKED_STRUCT( x )   \
	typedef struct _##x{
    #define END_PACKED_STRUCT(x)   \
        } x ; 
#elif  defined(__ICCARM__)
#pragma pack(1) 
	#define BEGIN_PACKED_STRUCT( x )   \
    	typedef struct _##x{
    #define END_PACKED_STRUCT(x)   \
        } x ; 
#else
   // #error  "UNKOWN build enviroment"

#endif
            

//@}

#endif /* __HTYPES_H */
