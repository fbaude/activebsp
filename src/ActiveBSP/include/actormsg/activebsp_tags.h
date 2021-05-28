#ifndef __ACTIVEBSP_TAGS_H__
#define __ACTIVEBSP_TAGS_H__

namespace activebsp
{

typedef enum
{
    TAG_INSTRUCTION = 0,
    TAG_ACTOR,

    TAG_FUTURE_BEGIN,
    TAG_FUTURE_END = 32768
} tags;

} // namespace activebsp

#endif // __ACTIVEBSP_TAGS_H__
