#ifndef GROVESTREAMSUPLOADER_H
#define GROVESTREAMSUPLOADER_H


class GroveStreamsUploader
{

private:
    static size_t download_handler( void *, size_t size, size_t nmemb, void * interface );

public:

    GroveStreamsUploader();
    ~GroveStreamsUploader();

    static uint32_t upload_all( void );
};

#endif // GROVESTREAMSUPLOADER_H
