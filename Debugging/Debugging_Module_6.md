# **Module 6: Tổng hợp và Ứng dụng 🧩**

#### **6.1. Ôn tập và Kết nối Kiến thức 🔗**

Hãy cùng điểm lại các chủ đề chính và mối liên hệ giữa chúng trong bối cảnh gỡ lỗi:

* **Module 1: Các loại Lỗi và Kỹ thuật Gỡ lỗi Chung**
  * **Kiến thức cốt lõi:** Phân loại lỗi (đặc tả, thiết kế, mã hóa), quy trình gỡ lỗi 5 giai đoạn (Testing, Stabilization, Localization, Correction, Verification), kỹ thuật Instrumentation (`printf`, `#ifdef DEBUG`, macros `__LINE__/__FILE__`).
  * **Kết nối:** Cung cấp khung lý thuyết và các kỹ thuật cơ bản để tiếp cận bất kỳ lỗi nào. Instrumentation là bước đầu tiên để thu thập thông tin.
* **Module 2: GDB - Trình gỡ lỗi GNU**
  * **Kiến thức cốt lõi:** Sử dụng GDB để kiểm soát thực thi (`run`, `next`, `step`), kiểm tra trạng thái (`print`, `display`), theo dõi luồng (`backtrace`), và đặt/quản lý các điểm dừng (`break`, `commands`).
  * **Kết nối:** GDB là công cụ mạnh mẽ nhất để thực hiện **Localization** (khoanh vùng lỗi) và **Correction** (sửa lỗi) bằng cách kiểm tra trực tiếp trạng thái chương trình. Nó bổ sung cho instrumentation bằng cách cung cấp khả năng kiểm soát tương tác.
* **Module 3: Các Công cụ Gỡ lỗi Khác (Phân tích tĩnh và động)**
  * **Kiến thức cốt lõi:** Phân tích tĩnh (`Splint`, `ctags`, `cxref`, `cflow`) để kiểm tra code mà không chạy. Phân tích động (`gprof`) để đánh giá hiệu suất.
  * **Kết nối:** Các công cụ phân tích tĩnh giúp phát hiện lỗi sớm trong giai đoạn **Coding** và  **Code Review** , giảm thời gian debug sau này. `gprof` giúp tối ưu hóa hiệu suất, một khía cạnh khác của "bug" trong hệ thống nhúng.
* **Module 4: Assertions**
  * **Kiến thức cốt lõi:** Sử dụng macro `assert()` để kiểm tra các giả định nội bộ của chương trình, bật/tắt bằng `NDEBUG`.
  * **Kết nối:** Assertions là một hình thức instrumentation chủ động, giúp phát hiện lỗi logic hoặc vi phạm giả định ngay tại điểm xảy ra, là công cụ tuyệt vời cho giai đoạn **Testing** và **Stabilization** trong phát triển.
* **Module 5: Gỡ lỗi Sử dụng Bộ nhớ (Memory Debugging)**
  * **Kiến thức cốt lõi:** Các vấn đề bộ nhớ động (rò rỉ, truy cập không hợp lệ, use-after-free, double-free) và công cụ chuyên biệt (`ElectricFence`, `Valgrind`).
  * **Kết nối:** `Valgrind` là công cụ tối thượng để phát hiện và khoanh vùng các lỗi bộ nhớ phức tạp, thường là nguyên nhân gốc rễ của các `SIGSEGV` khó hiểu. Nó cung cấp thông tin chi tiết mà GDB có thể không tự động hiển thị.

---

#### **6.2. Câu hỏi Tổng hợp và Tình huống ❓**

1. Tình huống (Daemon bị Crash ngẫu nhiên):
   Bạn có một daemon chạy trên thiết bị nhúng Linux. Nó hoạt động bình thường trong vài giờ hoặc vài ngày, sau đó đột nhiên bị crash với Segmentation fault. Lỗi này không tái hiện được dễ dàng.
   * Bạn sẽ biên dịch daemon của mình như thế nào để chuẩn bị cho việc gỡ lỗi?
   * Làm thế nào để bạn có thể thu thập thông tin về lỗi này sau khi nó xảy ra (khi không có terminal điều khiển)?
   * Khi bạn đã có thông tin về crash, bạn sẽ sử dụng công cụ nào để phân tích nó và tìm ra nguyên nhân gốc rễ? Nêu các lệnh cụ thể bạn sẽ dùng.
   * Nếu bạn nghi ngờ rò rỉ bộ nhớ là nguyên nhân khiến daemon crash sau thời gian dài, công cụ nào sẽ giúp bạn xác nhận điều đó?
2. **Kết hợp Công cụ:**
   * Mô tả một quy trình làm việc (workflow) gỡ lỗi mà bạn sẽ sử dụng để tìm và sửa một lỗi logic phức tạp trong một dự án C++ lớn, bắt đầu từ việc kiểm tra mã đến việc xác minh sửa lỗi. Hãy đề cập đến ít nhất 3 công cụ khác nhau từ giáo trình này.
   * Khi nào bạn sẽ ưu tiên dùng `ElectricFence` thay vì `Valgrind` để tìm lỗi truy cập bộ nhớ, và ngược lại?
3. **Thiết kế cho Debuggability:**
   * Bạn đang thiết kế một module phần mềm mới cho hệ thống nhúng. Hãy đề xuất ít nhất 3 cách bạn sẽ tích hợp các kỹ thuật "debugging" vào thiết kế và mã nguồn của module đó để giúp việc gỡ lỗi sau này dễ dàng hơn.
   * Tại sao việc tránh các side effects trong `assert()` lại quan trọng khi bạn muốn có một bản release "sạch"?
4. **Phân tích Output:**
   * Bạn chạy `gprof` trên chương trình của mình và thấy một hàm `process_data()` có "self seconds" rất cao. Điều này nói lên điều gì về hiệu suất của chương trình? Bạn sẽ làm gì tiếp theo để tối ưu hóa?
   * Bạn chạy `Valgrind` và thấy báo cáo `definitely lost: 1024 bytes in 1 blocks`. Điều này có ý nghĩa gì và bạn sẽ làm gì để sửa lỗi?

---

#### **6.3. Bài tập Thực hành Tổng hợp ✍️**

Bài tập này sẽ yêu cầu bạn kết hợp kiến thức từ nhiều Module để xây dựng một ứng dụng có lỗi và sau đó tự mình gỡ lỗi nó một cách có hệ thống.

**Bài tập: Ứng dụng Quản lý Danh sách Nhiệm vụ (Task Manager) có Lỗi**

Viết một chương trình C++ (`task_manager.cpp`) mô phỏng một ứng dụng quản lý danh sách nhiệm vụ đơn giản. Chương trình này sẽ chứa một số lỗi cố ý (hoặc không cố ý) mà bạn sẽ phải tìm và sửa.

**Yêu cầu chương trình:**

1. **Cấu trúc Dữ liệu:**
   * Định nghĩa một `struct Task { int id; char description[100]; bool completed; };`
   * Sử dụng một mảng động (`Task* tasks_array;` hoặc `std::vector<Task>`) để lưu trữ các nhiệm vụ.
2. **Các Chức năng:**
   * `add_task(const char* desc)`: Thêm một nhiệm vụ mới. Gán `id` tăng dần. Cấp phát bộ nhớ cho nhiệm vụ.
   * `complete_task(int id)`: Đánh dấu một nhiệm vụ là hoàn thành.
   * `delete_task(int id)`: Xóa một nhiệm vụ khỏi danh sách. Giải phóng bộ nhớ.
   * `list_tasks()`: In ra tất cả các nhiệm vụ.
   * `cleanup_all_tasks()`: Xóa tất cả các nhiệm vụ và giải phóng bộ nhớ.
3. **Lỗi Cố ý (hoặc để tự phát hiện):**
   * **Rò rỉ Bộ nhớ:** Trong `add_task`, không giải phóng bộ nhớ khi mảng động cần `realloc` nhưng thất bại.
   * **Truy cập Ngoài biên:** Trong `complete_task` hoặc `delete_task`, không kiểm tra `id` hợp lệ, dẫn đến truy cập ngoài biên mảng.
   * **Use-after-free:** Trong `delete_task`, sau khi `free` một nhiệm vụ, cố gắng truy cập lại `description` của nó.
   * **Double-free:** Trong `cleanup_all_tasks`, gọi `free` hai lần cho cùng một khối bộ nhớ.
   * **Lỗi Logic:** Trong `list_tasks`, vòng lặp in sai số lượng nhiệm vụ.
4. **Giao diện Dòng lệnh:**
   * Sử dụng `getopt_long()` để xử lý các lệnh:
     * `--add <description>`: Thêm nhiệm vụ.
     * `--complete <id>`: Hoàn thành nhiệm vụ.
     * `--delete <id>`: Xóa nhiệm vụ.
     * `--list`: Liệt kê nhiệm vụ.
     * `--cleanup`: Xóa tất cả nhiệm vụ.
     * `--help`: Hướng dẫn sử dụng.
5. **Logging/Debugging:**
   * Tích hợp `syslog()` hoặc một hàm log file tùy chỉnh (như trong Module 6 của giáo trình "The Linux Environment") để ghi lại các sự kiện quan trọng (thêm/xóa nhiệm vụ, lỗi).
   * Sử dụng `assert()` để kiểm tra các giả định quan trọng (ví dụ: con trỏ không null trước khi dereference). Biên dịch có/không có `-DNDEBUG`.
   * Đảm bảo chương trình có thể biên dịch với `-g`.

**Quy trình Gỡ lỗi (Bạn sẽ thực hiện):**

1. **Viết code `task_manager.cpp`:** Cố tình thêm các lỗi trên hoặc để chúng tự xuất hiện.
2. **Biên dịch và Chạy Thử:**
   * Biên dịch với `g++ task_manager.cpp -o task_manager -g -lm` (và có thể `-DNDEBUG` để kiểm tra hành vi assertion).
   * Chạy chương trình với các lệnh khác nhau (thêm nhiều nhiệm vụ, xóa, hoàn thành) để cố gắng tái hiện lỗi.
3. **Chẩn đoán với `Valgrind`:**
   * Chạy chương trình dưới `Valgrind`: `valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./task_manager [các lệnh]`
   * **Mục tiêu:** Xác định tất cả các lỗi bộ nhớ (rò rỉ, truy cập không hợp lệ, double-free) và vị trí của chúng.
4. **Khoanh vùng với `GDB`:**
   * Nếu `Valgrind` báo cáo lỗi truy cập bộ nhớ hoặc chương trình crash, hãy chạy chương trình trong GDB.
   * Đặt breakpoint tại các dòng mà `Valgrind` báo lỗi.
   * Sử dụng `next`, `step`, `print`, `backtrace` để hiểu nguyên nhân gốc rễ.
   * **Mục tiêu:** Xác định chính xác lỗi logic hoặc lỗi con trỏ gây ra vấn đề.
5. **Sửa lỗi:** Sửa từng lỗi một trong mã nguồn.
6. **Xác minh:** Biên dịch lại và chạy lại với `Valgrind` và GDB để đảm bảo tất cả các lỗi đã được khắc phục.
