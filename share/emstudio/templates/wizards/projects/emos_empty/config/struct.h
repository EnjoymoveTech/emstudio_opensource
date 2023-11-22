// file test1.h
#pragma pack(4)
typedef struct _SampleStruct1
{
    tUInt64 element1;
    tUInt32 element2[2];
    tUInt32 element3;
    tUInt32 element4;
    tChar element5[20];
} SampleStruct1;
#pragma pack()

#pragma pack(4)
typedef struct _SampleStruct2
{
    tUInt32 element1;
    tFloat32 element2;
    tFloat32 element3;
    tFloat32 element4;
    tFloat32 element5;
} SampleStruct2;
#pragma pack()

#pragma pack(4)
typedef struct _SampleStruct3
{
    tUInt64 element1;
    tUInt32 element2;
    SampleStruct1 element3[5];
} SampleStruct3;
#pragma pack()

