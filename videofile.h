#ifndef VIDEOFILE_H
#define VIDEOFILE_H

#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QDateTime>
#include <QCache>
#include "typedef.h"

enum InterpolationMode
{
    NearestNeighborInterpolation,
    BiLinearInterpolation,
    InterstitialInterpolation
};

typedef struct {
    int identifier;
    int bitsPerSample;
    int bitsPerPixelNominator;
    int bitsPerPixelDenominator;
    int subsamplingHorizontal;
    int subsamplingVertical;
    bool planar;
} formatProp_t;

class CacheIdx
 {
 public:
     CacheIdx(const QString &name, const unsigned int idx) { fileName=name; frameIdx=idx; }

     QString fileName;
     unsigned int frameIdx;
 };

 inline bool operator==(const CacheIdx &e1, const CacheIdx &e2)
 {
     return e1.fileName == e2.fileName && e1.frameIdx == e2.frameIdx;
 }

 inline uint qHash(const CacheIdx &cIdx)
 {
     uint tmp = qHash(cIdx.fileName) ^ qHash(cIdx.frameIdx);
     return tmp;
 }


class VideoFile : public QObject
{
public:
    explicit VideoFile(const QString &fname, QObject *parent = 0);

    ~VideoFile();

public:

    virtual void getOneFrame( void* &frameData, unsigned int frameIdx, int width, int height, YUVCPixelFormatType srcFormat );

    virtual void extractFormat(int* width, int* height, YUVCPixelFormatType* cFormat, int* numFrames, double* frameRate) = 0;

    virtual void refreshNumberFrames(int* numFrames, int width, int height, YUVCPixelFormatType cFormat) = 0;

    virtual QString fileName();

    void clearCache();

    //  methods for querying file information
    virtual QString getPath() {return p_path;}
    virtual QString getCreatedtime() {return p_createdtime;}
    virtual QString getModifiedtime() {return p_modifiedtime;}

    void setInterPolationMode(InterpolationMode newMode) { p_interpolationMode = newMode; }
    InterpolationMode getInterpolationMode() { return p_interpolationMode; }

    static QCache<CacheIdx, QByteArray> frameCache;

protected:
    QFile *p_srcFile;
    QFileInfo fileInfo;

    QString p_path;
    QString p_createdtime;
    QString p_modifiedtime;

    QByteArray p_tmpBufferYUV;
    QByteArray p_tmpBufferYUV444;

    InterpolationMode p_interpolationMode;

    virtual unsigned int getFileSize();

    virtual int getFrames( QByteArray *targetBuffer, unsigned int frameIndex, unsigned int frames2read, int width, int height, YUVCPixelFormatType srcPixelFormat ) = 0;

    void convert2YUV444(QByteArray *sourceBuffer, YUVCPixelFormatType srcPixelFormat, int lumaWidth, int lumaHeight, QByteArray *targetBuffer);
    void convertYUV2RGB(QByteArray *sourceBuffer, QByteArray *targetBuffer, YUVCPixelFormatType targetPixelFormat);

    int verticalSubSampling(YUVCPixelFormatType pixelFormat);
    int horizontalSubSampling(YUVCPixelFormatType pixelFormat);
    int bitsPerSample(YUVCPixelFormatType pixelFormat);
    int bytesPerFrame(int width, int height, YUVCPixelFormatType cFormat);
    bool isPlanar(YUVCPixelFormatType pixelFormat);
};

#endif // VIDEOFILE_H
