#ifndef TEXT_STREAM_OBJECT_BASE_H
#define TEXT_STREAM_OBJECT_BASE_H

struct TextStreamSettings
{
    bool show_topic_id;
    bool show_timestamp;
};

class TextStreamObjectBase
{
private:
public:
    TextStreamObjectBase();
    TextStreamObjectBase() {}
};

#endif  // TEXT_STREAM_OBJECT_BASE_H
