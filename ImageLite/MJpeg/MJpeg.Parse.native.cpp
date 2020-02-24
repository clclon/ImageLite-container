
#include "../ImageLite.h"
#include "NVJpegDecoder.Error.h"

namespace ImageLite
{
#   include "MJpeg.Table.h"

    void foreach_native(
                const char *b,
                int32_t sz,
                int32_t i,
                int32_t & spos,
                int32_t & epos,
                MJpeg::StageType & state)
    {
        for (; (i + 1) < sz; i++)
        {
            MJpeg::StageType m;

            switch(state)
            {
                case MJpeg::StageType::MST_UNKNOWN:
                    {
                        if ((m = check_jpeg_tag(b[i + 0], b[i + 1], b[i + 2])) != MJpeg::StageType::MST_BEGIN)
                            continue;
                        break;
                    }
                case MJpeg::StageType::MST_BEGIN:
                    {
                        if ((m = check_jpeg_tag(b[i + 0], b[i + 1], 0)) != MJpeg::StageType::MST_END)
                            continue;
                        break;
                    }
                case MJpeg::StageType::MST_END:
                    return;
            }
            switch(m)
            {
                case MJpeg::StageType::MST_BEGIN:
                    {
                        state = MJpeg::StageType::MST_BEGIN;
                        spos = i;
                        i += sizeof(JFIFHEAD);
                        continue;
                    }
                case MJpeg::StageType::MST_END:
                    {
                        state = MJpeg::StageType::MST_END;
                        epos = (i + 2);
                        return;
                    }
                case MJpeg::StageType::MST_UNKNOWN:
                    break;
            }
        }
    }
}
