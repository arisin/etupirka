#pragma once

#include <vector>
#include <array>

#include <Magick++.h>

namespace
{
  constexpr int cv_imwrite_jpeg_quality =
#if CV_MAJOR_VERSION < 3
    CV_IMWRITE_JPEG_QUALITY
#else
    cv::IMWRITE_JPEG_QUALITY
#endif
  ;
}

namespace arisin
{
  namespace etupirka
  {
    struct frame_packet_t final
    {
      using sequence_id_t = uint8_t;
      
      uint16_t real_data_size;
      uint8_t capture_id;
      sequence_id_t sequence_id;
      
      static constexpr size_t info_size = sizeof(real_data_size) - sizeof(capture_id) - sizeof(sequence_id);
      static constexpr size_t data_size = 65506 - info_size;
      static constexpr size_t this_size = info_size + data_size;
      
      uint8_t data[data_size];
      
      inline void set_data(const cv::Mat& m, const int jpeg_quality = 60)
      {
        Magick::Image i(m.cols, m.rows, "BGR", Magick::CharPixel, reinterpret_cast<char*>(m.data));
        Magick::Blob buffer;
        i.magick("JPEG");
        i.quality(jpeg_quality);
        i.write(&buffer);
        
        if(buffer.length() > data_size)
          throw std::runtime_error("encoded frame size is over.");
        
        std::copy(reinterpret_cast<const uint8_t*>(buffer.data()), reinterpret_cast<const uint8_t*>(buffer.data()) + buffer.length(), &data[0]);
        real_data_size = buffer.length();
      }
      
      inline const uint8_t* data_begin() const { return &data[0]; }
      inline const uint8_t* data_end() const { return data_begin() + size_t(data_size); }
      
      using mutate_array_t = std::array<uint8_t, this_size>;
      
      inline const mutate_array_t& mutate_to_const_array() const
      { return *reinterpret_cast<const mutate_array_t*>(this); }
      
      inline mutate_array_t& mutate_to_array() const
      { return *reinterpret_cast<mutate_array_t*>(const_cast<frame_packet_t*>(this)); }
      
      using vector_t = std::vector<uint8_t>;
      
      inline vector_t to_vector() const
      { return vector_t(data_begin(), data_end());}
    };
  }
}
