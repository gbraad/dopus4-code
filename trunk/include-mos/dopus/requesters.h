#ifndef DOPUS_REQUESTERS_H
#define DOPUS_REQUESTERS_H
struct RequesterObject {
    struct RequesterObject *ro_next;     
    ULONG ro_type;                       
    APTR ro_data;                        
} __attribute__((packed));
struct RequesterBase {
    short rb_width,rb_height;            
    short rb_widthfine,rb_heightfine;    

    short rb_leftoffset,rb_topoffset;    

    char rb_fg,rb_bg;                    
    char rb_shine,rb_shadow;             
    struct TextFont *rb_font;            
    char *rb_screenname;                 
    struct Screen *rb_screen;            
    char *rb_title;                      

    struct DOpusRemember *rb_memory;     
    struct RequesterObject *rb_objects;  
    struct Window *rb_window;            

    ULONG rb_flags;                      
    ULONG rb_privateflags;               
    struct StringExtend *rb_extend;      
    ULONG rb_idcmpflags;                 

    APTR rb_privatedata;                 
    char **rb_string_table;              
} __attribute__((packed));



#define RBF_BORDERS         1          
#define RBF_CLOSEGAD        2          
#define RBF_STRINGS         4          
#define RBF_WINDOWCENTER    8          



#define RBP_CLOSEFONT       1          




typedef struct RequesterObject_Text {
    short ot_left,ot_top;                
    char *ot_text;                       
} Object_Text;




typedef struct RequesterObject_Border {
    short ob_left,ob_top;                
    short ob_width,ob_height;            
    short ob_type;                       
    short ob_textpos;                    
    char *ob_text;                       
} Object_Border;




typedef struct RequesterObject_Gadget {
    struct Gadget og_gadget;             
    short og_textpos;                    
    char *og_text;                       
    char og_type;                        
} Object_Gadget;




typedef struct RequesterObject_ListView {
    struct DOpusListView ol_listview;    
} Object_ListView;




#define RO_Dummy      (TAG_USER + 99)

#define RO_Type       (RO_Dummy + 0x01)    

#define RO_Left       (RO_Dummy + 0x02)    
#define RO_Top        (RO_Dummy + 0x03)    
#define RO_LeftFine   (RO_Dummy + 0x04)    
#define RO_TopFine    (RO_Dummy + 0x05)    

#define RO_Width      (RO_Dummy + 0x06)    
#define RO_Height     (RO_Dummy + 0x07)    
#define RO_WidthFine  (RO_Dummy + 0x08)    
#define RO_HeightFine (RO_Dummy + 0x09)    

#define RO_Text       (RO_Dummy + 0x0a)    
#define RO_TextPos    (RO_Dummy + 0x0b)    

#define RO_GadgetID   (RO_Dummy + 0x0c)    
#define RO_GadgetType (RO_Dummy + 0x0d)    

#define RO_BorderType (RO_Dummy + 0x0e)    

#define RO_ListViewID (RO_Dummy + 0x0f)    

#define RO_StringBuf  (RO_Dummy + 0x10)    
#define RO_StringLen  (RO_Dummy + 0x11)    
#define RO_StringUndo (RO_Dummy + 0x12)    

#define RO_BoolOn     (RO_Dummy + 0x13)    

#define RO_ChkCenter  (RO_Dummy + 0x14)    

#define RO_TextNum    (RO_Dummy + 0x15)    

#define RO_HighRecess (RO_Dummy + 0x16)    




#define OBJECT_TEXT      1 
#define OBJECT_BORDER    2 
#define OBJECT_GADGET    3 
#define OBJECT_LISTVIEW  4 




#define TEXTPOS_ABSOLUTE 1 
#define TEXTPOS_CENTER   2 
#define TEXTPOS_LEFT     3 
#define TEXTPOS_RIGHT    4 
#define TEXTPOS_ABOVE    5 
#define TEXTPOS_BELOW    6 

#define TEXTPOS_F_NOUSCORE 32768  



#define BORDER_NONE      0 
#define BORDER_NORMAL    1 
#define BORDER_RECESSED  2 
#define BORDER_GROUP     3 
#define BORDER_STRING    4 




#define GADGET_BOOLEAN   0 
#define GADGET_STRING    1 
#define GADGET_PROP      2 
#define GADGET_CHECK     3 
#define GADGET_CYCLE     4 
#define GADGET_RADIO     5 
#define GADGET_GLASS     6 
#define GADGET_INTEGER   7 
#define GADGET_TINYGLASS 8 

#endif 


