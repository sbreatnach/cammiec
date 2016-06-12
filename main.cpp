#include <iostream>
#include <string>
#include <boost/lexical_cast.hpp>
#include <opencv2/opencv.hpp>
#include "mjpeg_server.hpp"

int
main(int argc, char* argv[])
{
  try
  {
    // Check command line arguments.
    if (argc != 5)
    {
      std::cerr << "Usage: " << argv[0] << " <address> <port> <threads> <doc_root>\n";
      std::cerr << "try:" << std::endl;
      std::cerr << "   " << argv[0] << " 0.0.0.0 9090 8 .\n";
      return 1;
    }
    using namespace http::server;
    // Run server in background thread.
    std::size_t num_threads = boost::lexical_cast<std::size_t>(argv[3]);
    server_ptr s(init_streaming_server(argv[1], argv[2], argv[4], num_threads));
    streamer_ptr stmr(new streamer);
    register_streamer(s, stmr, "/stream_0");
    s->start();
    std::cout << "Visit:\n" << argv[1] << ":" << argv[2] << "/stream_0" << std::endl;

    cv::VideoCapture stream1(0);
    if (!stream1.isOpened()) {
      std::cout << "cannot open camera";
      return -2;
    }

    while (true) {
      cv::Mat cameraFrame;
      stream1.read(cameraFrame);

      bool wait = false; //don't wait for there to be more than one webpage looking at us.
      int quality = 80;
      stmr->post_image(cameraFrame,quality, wait);
      boost::this_thread::sleep(boost::posix_time::milliseconds(5000)); //10 fps for ease on my eyes
    }
  } catch (std::exception& e)
  {
    std::cerr << "exception: " << e.what() << "\n";
  }
  return 0;
}
