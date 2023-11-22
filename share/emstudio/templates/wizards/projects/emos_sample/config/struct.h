#define MAX_ARRAY_TEST 5  // max array size
#define MACRO_TEST_NAME "test"  // name


// file common.h
enum SampleEnum1
{
    element1_1 = 1,  // this is element1_1 in SampleEnum1
    element1_2 = 2,
    element1_3 = 4,
    element1_4 = 8,
    element1_5 = 16,
};

enum SampleEnum2
{
    element2_1 = 0,
    element2_2,
    element2_3,
    element2_4,
    element2_5,
};


// file common.h
#pragma pack(4)
typedef struct _SampleStruct1
{
    tUInt64 element1;  // this is element1 in SampleStruct1
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

#pragma pack(4)
typedef struct _ObjectResult
{
  tUInt32 res[2];
} ObjectResult;
#pragma pack()

