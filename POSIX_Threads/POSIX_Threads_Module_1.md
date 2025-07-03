# **Giáo trình: POSIX Threads 🧵**

**Mục tiêu của Giáo trình 🎯**

Sau khi hoàn thành giáo trình này, bạn sẽ có thể:

* Hiểu rõ sự khác biệt giữa tiến trình và luồng, cũng như ưu nhược điểm của luồng.
* Tạo và quản lý các luồng mới trong một tiến trình sử dụng API POSIX Threads.
* Đồng bộ hóa quyền truy cập dữ liệu chung giữa các luồng bằng Semaphores và Mutexes.
* Điều chỉnh các thuộc tính của luồng, bao gồm trạng thái tách rời (detached state) và chính sách lập lịch (scheduling policy).
* Kiểm soát việc chấm dứt luồng (canceling threads) và xử lý các yêu cầu hủy.
* Nắm vững các vấn đề thường gặp khi làm việc với nhiều luồng và cách phòng tránh chúng.
* Áp dụng kiến thức về luồng để phát triển các ứng dụng hiệu suất cao, phản hồi nhanh trong môi trường Linux và nhúng.

---

### **Cấu trúc Giáo trình 📚**

Giáo trình này sẽ được chia thành các Modules nhỏ để dễ dàng theo dõi và tiếp thu:

* **Module 1: Luồng là gì? (What Is a Thread?)**
* **Module 2: Tạo và Quản lý Luồng Cơ bản**
* **Module 3: Đồng bộ hóa Luồng (Synchronization)**
* **Module 4: Thuộc tính Luồng (Thread Attributes)**
* **Module 5: Hủy một Luồng (Canceling a Thread)**
* **Module 6: Làm việc với nhiều Luồng (Threads in Abundance)**
* **Module 7: Luyện tập Tổng hợp & Ứng dụng**

Mỗi Module sẽ bao gồm:

* **Lý thuyết chi tiết:** Giải thích các khái niệm, hàm, và cơ chế.
* **Ví dụ Code (C++):** Minh họa cách sử dụng các hàm.
* **Liên hệ với Embedded Linux:** Giải thích tầm quan trọng và ứng dụng trong hệ thống nhúng.
* **Câu hỏi Tự đánh giá:** Giúp bạn kiểm tra mức độ hiểu bài.
* **Bài tập Thực hành:** Các bài tập coding để bạn áp dụng kiến thức.

Hãy bắt đầu với Module đầu tiên!

---

### **Module 1: Luồng là gì? (What Is a Thread?) 🧵**

Module này sẽ giúp bạn hiểu rõ khái niệm cơ bản về luồng, tầm quan trọng của nó và sự khác biệt với tiến trình.

#### **1.1. Định nghĩa Luồng (What is a Thread?) 🤔**

* **Lý thuyết:**
  * Một  **luồng (thread)** , hay còn gọi là  **luồng điều khiển (thread of execution)** , là một đơn vị thực thi bên trong một tiến trình (process).
  * Trong khi một **tiến trình** là một chương trình đang chạy với không gian bộ nhớ độc lập, một luồng là một phần của tiến trình đó.
  * Tất cả các chương trình bạn đã thấy cho đến nay đều chạy như một tiến trình đơn luồng (single-threaded process), tức là chỉ có một luồng thực thi duy nhất. Tuy nhiên, một tiến trình có thể có nhiều hơn một luồng.
  * **Phân biệt với `fork()`:**
    * Khi một tiến trình gọi `fork()`, một **bản sao hoàn chỉnh** của tiến trình đó được tạo ra, với  **không gian địa chỉ bộ nhớ riêng** , các biến riêng, và một  **PID mới** . Tiến trình con này được lập lịch độc lập và thực thi gần như độc lập với tiến trình cha.
    * Khi bạn tạo một **luồng mới** trong một tiến trình, luồng mới đó có **ngăn xếp (stack) riêng** (và do đó là các biến cục bộ riêng), nhưng nó **chia sẻ** các tài nguyên sau với các luồng khác trong cùng tiến trình:
      * Các biến toàn cục (global variables).
      * Các file descriptor đang mở.
      * Các trình xử lý tín hiệu (signal handlers).
      * Trạng thái thư mục hiện tại (current directory state).
      * Không gian địa chỉ bộ nhớ (code segment, data segment, heap).
* **POSIX Threads (pthreads):**
  * Khái niệm luồng đã tồn tại từ lâu, nhưng việc triển khai rất khác nhau giữa các hệ thống UNIX. Tiêu chuẩn **POSIX 1003.1c** (cụ thể là thư viện pthreads) đã thay đổi điều đó, cung cấp một API chuẩn hóa cho việc lập trình luồng, đảm bảo khả năng di động trên các hệ điều hành tuân thủ POSIX như Linux.
  * Trên Linux, triển khai pthreads hiện đại nhất và hiệu suất cao nhất là  **NPTL (Native POSIX Thread Library)** , khác biệt so với các thư viện cũ hơn như LinuxThreads.

#### **1.2. Ưu điểm và Hạn chế của Luồng (Advantages and Drawbacks) ✅❌**

**Ưu điểm (Advantages) ✅:**

* **Hiệu suất (Performance):**
  * Các luồng có thể thực sự chạy **song song** trên các bộ xử lý  **đa lõi (multi-core CPUs)** , giúp tăng tốc độ xử lý cho các tác vụ có thể phân chia.
  * Trong hệ thống đơn lõi, Kernel vẫn có thể xen kẽ việc thực thi giữa các luồng (time-slicing), tạo ảo giác đồng thời.
* **Chia sẻ tài nguyên (Resource Sharing):** Vì các luồng trong cùng một tiến trình chia sẻ chung không gian bộ nhớ và tài nguyên, việc giao tiếp và chia sẻ dữ liệu giữa chúng dễ dàng và nhanh chóng hơn nhiều so với các cơ chế Giao tiếp Liên Tiến trình (IPC) giữa các tiến trình riêng biệt.
* **Chi phí thấp (Lower Overhead):** Tạo và chuyển đổi ngữ cảnh giữa các luồng nhanh và ít tốn tài nguyên hơn đáng kể so với việc tạo và chuyển đổi giữa các tiến trình, vì không cần tạo/sao chép toàn bộ không gian địa chỉ bộ nhớ.
* **Phản hồi nhanh (Responsiveness):** Trong các ứng dụng có giao diện người dùng hoặc cần xử lý các tác vụ dài mà không làm treo toàn bộ chương trình, bạn có thể chạy tác vụ nặng trong một luồng riêng, giữ cho luồng chính (main thread) luôn phản hồi.
* **Phù hợp cho các ứng dụng có dữ liệu liên kết chặt chẽ:** Ví dụ: máy chủ cơ sở dữ liệu đa luồng phục vụ nhiều client, nơi cần truy cập chung một lượng lớn dữ liệu với độ đồng bộ cao.

**Hạn chế (Drawbacks) ❌:**

* **Đồng bộ hóa phức tạp (Synchronization Complexity):** Đây là thách thức lớn nhất. Việc chia sẻ bộ nhớ đòi hỏi lập trình viên phải sử dụng các cơ chế đồng bộ hóa (ví dụ: mutex, semaphore, condition variables) để bảo vệ dữ liệu chung và tránh  **điều kiện tranh chấp (race conditions)** , nơi nhiều luồng cùng cố gắng truy cập/sửa đổi dữ liệu, dẫn đến kết quả không chính xác.
* **Deadlock (Tắc nghẽn):** Khi các luồng chờ đợi tài nguyên mà các luồng khác đang giữ, có thể dẫn đến tình trạng tắc nghẽn, làm treo toàn bộ ứng dụng.
* **Lỗi khó tìm (Hard-to-find Bugs):** Các lỗi liên quan đến đồng bộ hóa và thời gian thường không xảy ra một cách nhất quán (chỉ xảy ra dưới những điều kiện thời gian cụ thể), khiến chúng rất khó tái hiện (reproduce) và gỡ lỗi (debug).
* **Khó kiểm soát tài nguyên:** Mặc dù luồng nhẹ hơn, nhưng tạo quá nhiều luồng vẫn có thể gây overhead cho hệ thống do việc quản lý và lập lịch.

#### **1.3. Thực thi Đồng thời (Simultaneous Execution) 🚀**

* **Lý thuyết:** Khi bạn tạo nhiều luồng, hệ điều hành sẽ lập lịch để chúng thực thi đồng thời.
  * Trên CPU đa lõi, các luồng có thể thực sự chạy **song song** trên các lõi khác nhau.
  * Trên CPU đơn lõi, hệ điều hành sẽ sử dụng kỹ thuật  **chia sẻ thời gian (time-slicing)** , xen kẽ việc thực thi giữa các luồng rất nhanh, tạo ra ảo giác rằng chúng đang chạy đồng thời.
  * **Vấn đề Busy-Wait (Thăm dò bận rộn):** Là một phương pháp không hiệu quả để đồng bộ hóa luồng, trong đó một luồng liên tục kiểm tra một biến chung trong vòng lặp (`while (condition) {}`) mà không nhường CPU. Điều này tiêu tốn tài nguyên CPU một cách lãng phí. Thay vào đó, luồng nên "ngủ" hoặc sử dụng các cơ chế đồng bộ hóa chặn (blocking synchronization mechanisms) như semaphore/mutex để chờ sự kiện.
* **Ví dụ (`thread2.c` / `thread2.cpp`):** Minh họa việc các luồng thay phiên nhau in ký tự '1' và '2' bằng cách thay đổi một biến toàn cục `run_now`. Code này sử dụng busy-wait (`if (run_now == X) { ... } else { sleep(1); }`) để đồng bộ hóa, thể hiện sự kém hiệu quả.
* **Liên hệ Embedded Linux:** Trong nhúng, thực thi đồng thời là lý do chính để sử dụng luồng. Tuy nhiên, việc tránh busy-wait và sử dụng các cơ chế đồng bộ hóa hiệu quả là tối quan trọng để tiết kiệm chu kỳ CPU và năng lượng trên các bộ vi xử lý tài nguyên hạn chế.

---

### **Câu hỏi Tự đánh giá Module 1 🤔**

1. Giải thích sự khác biệt cơ bản giữa một **tiến trình (process)** và một **luồng (thread)** về mặt tài nguyên mà chúng sử dụng và chia sẻ.
2. Nêu ít nhất ba ưu điểm chính của việc sử dụng luồng so với tiến trình khi thiết kế một ứng dụng.
3. Nêu ít nhất hai thách thức hoặc hạn chế chính khi viết chương trình đa luồng. Điều gì là vấn đề khó tìm lỗi nhất trong số đó?
4. "POSIX Threads" là gì và tại sao tiêu chuẩn này lại quan trọng đối với lập trình luồng?
5. Giải thích khái niệm "Busy-Wait" trong lập trình luồng. Tại sao nó được coi là một phương pháp đồng bộ hóa kém hiệu quả?

---

### **Bài tập Thực hành Module 1 ✍️**

1. **Chương trình "Process vs. Thread Overhead":**
   * Viết hai chương trình C++ riêng biệt:

     * **`process_overhead.cpp`:** Trong `main()`, tạo một vòng lặp chạy 100 lần. Trong mỗi lần lặp, `fork()` một tiến trình con, tiến trình con đó chỉ in ra PID của nó và thoát ngay lập tức (`exit(0)`). Tiến trình cha `wait()` cho con kết thúc.
     * **`thread_overhead.cpp`:** Trong `main()`, tạo một vòng lặp chạy 100 lần. Trong mỗi lần lặp, tạo một luồng con (`pthread_create()`), luồng con đó chỉ in ra ID của nó và thoát (`pthread_exit(NULL)`). Luồng cha `pthread_join()` cho con kết thúc.
   * Biên dịch cả hai chương trình (nhớ `-pthread` cho `thread_overhead.cpp`).
   * **Mục tiêu:** Sử dụng lệnh `time` của shell để đo thời gian thực thi của cả hai chương trình:
     **Bash**

     ```
     g++ process_overhead.cpp -o process_overhead
     g++ thread_overhead.cpp -o thread_overhead -pthread

     time ./process_overhead
     time ./thread_overhead
     ```
   * So sánh thời gian thực thi và giải thích tại sao một chương trình lại nhanh hơn đáng kể so với chương trình kia.
2. **Chương trình "Busy-Wait Demo":**
   * Viết một chương trình C++ (`busy_wait_demo.cpp`) mô phỏng ví dụ `thread2.c` từ bài học, có hai luồng và một biến toàn cục `run_now`.
   * **`main` thread:** Kiểm tra `run_now == 1`, in '1', sau đó đặt `run_now = 2`. Nếu không, `sleep(1)`.
   * **`thread_function`:** Kiểm tra `run_now == 2`, in '2', sau đó đặt `run_now = 1`. Nếu không, `sleep(1)`.
   * Chạy chương trình và quan sát output.
   * **Thử thách:**
     * Xóa tất cả các lệnh `sleep(1)` trong cả hai luồng và chạy lại chương trình.
     * Sử dụng `top` hoặc `htop` để quan sát mức sử dụng CPU của chương trình. Giải thích tại sao CPU lại bị tiêu tốn nhiều như vậy ngay cả khi không có `sleep()`.
