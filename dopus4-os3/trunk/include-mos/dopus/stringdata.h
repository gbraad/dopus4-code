#ifndef DOPUS_STRINGDATA_H
#define DOPUS_STRINGDATA_H
struct DefaultString {
    USHORT string_id;                
    char * __attribute__((aligned(2))) string;                    
} __attribute__((packed));
struct StringData {
    struct DefaultString             
        *default_table;
    int string_count;                

    char **string_table;             

    char *string_buffer;             
    ULONG string_size;               

    int min_version;                 

    struct Library *LocaleBase;      // JRZ
    struct Catalog *catalog;         // JRZ
} __attribute__((packed));




#define ID_OSTR MAKE_ID('O','S','T','R')
#define ID_STHD MAKE_ID('S','T','H','D')
#define ID_STRN MAKE_ID('S','T','R','N')

typedef struct StringFile_Header {
    ULONG header_id;         
    ULONG header_size;
    ULONG version;
    ULONG stringcount;
    ULONG flags;
    char  language[20];
} StringHeader;

typedef struct StringFile_String {
    ULONG chunk_id;          
  ULONG chunk_size;
  ULONG string_id;
} String;

#endif 
