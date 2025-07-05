# 📘 **Giáo trình: POSIX Threads 🧵**

## 🎯 **Mục tiêu của Giáo trình**

Sau khi hoàn thành giáo trình này, bạn sẽ có thể:

* Hiểu rõ sự khác biệt giữa tiến trình và luồng, cũng như ưu nhược điểm của luồng
* Tạo và quản lý các luồng mới trong một tiến trình sử dụng API POSIX Threads
* Đồng bộ hóa quyền truy cập dữ liệu chung giữa các luồng bằng Semaphores và Mutexes
* Điều chỉnh các thuộc tính của luồng, bao gồm trạng thái tách rời (detached state) và chính sách lập lịch (scheduling policy)
* Kiểm soát việc chấm dứt luồng (canceling threads) và xử lý các yêu cầu hủy
* Nắm vững các vấn đề thường gặp khi làm việc với nhiều luồng và cách phòng tránh chúng
* Áp dụng kiến thức về luồng để phát triển các ứng dụng hiệu suất cao, phản hồi nhanh trong môi trường Linux và nhúng

## 📚 **Cấu trúc Giáo trình**

Giáo trình này sẽ được chia thành các Modules nhỏ để dễ dàng theo dõi và tiếp thu:

* **Module 1:** Luồng là gì? (What Is a Thread?)
* **Module 2:** Tạo và Quản lý Luồng Cơ bản
* **Module 3:** Đồng bộ hóa Luồng (Synchronization)
* **Module 4:** Thuộc tính Luồng (Thread Attributes)
* **Module 5:** Hủy một Luồng (Canceling a Thread)
* **Module 6:** Làm việc với nhiều Luồng (Threads in Abundance)
* **Module 7:** Luyện tập Tổng hợp & Ứng dụng

🔍 Mỗi Module sẽ bao gồm:

* 📖 Lý thuyết chi tiết: Giải thích các khái niệm, hàm, và cơ chế
* 💻 Ví dụ Code (C++): Minh họa cách sử dụng các hàm
* 🔧 Liên hệ với Embedded Linux
* ❓ Câu hỏi Tự đánh giá
* 🧪 Bài tập Thực hành

---

## 🧵 **Module 1: Luồng là gì? (What Is a Thread?)**

Module này sẽ giúp bạn hiểu rõ khái niệm cơ bản về luồng, tầm quan trọng của nó và sự khác biệt với tiến trình.

---

## 🤔 **1.1. Định nghĩa Luồng (What is a Thread?)**

### 📖 Lý thuyết

* Một  **luồng (thread)** , hay còn gọi là  **luồng điều khiển (thread of execution)** , là một đơn vị thực thi bên trong một tiến trình (process).
* Trong khi một **tiến trình** là một chương trình đang chạy với không gian bộ nhớ độc lập, một luồng là một phần của tiến trình đó.
* Tất cả các chương trình bạn đã thấy cho đến nay đều chạy như một tiến trình  **đơn luồng (single-threaded process)** , tức là chỉ có một luồng thực thi duy nhất.

  → Tuy nhiên, một tiến trình có thể có nhiều hơn một luồng.

### 🔄 Phân biệt `fork()` vs `pthread_create()`

| Đặc điểm                | `fork()` – Tạo tiến trình con                                                                                                                     | `pthread_create()` – Tạo luồng mới trong tiến trình               |
| --------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------- |
| Không gian bộ nhớ        | Có**địa chỉ bộ nhớ riêng**, sao chép hoàn toàn                                                                                          | Có**stack riêng**, nhưng **chia sẻ** không gian bộ nhớ |
| Tài nguyên                | **vẫn chia sẻ một số tài nguyên như file descriptor, signal handler, working directory…** thông qua cơ chế sao chép hoặc tham chiếu | Chia sẻ file descriptors, biến global, signal handlers                  |
| Nhận dạng                 | Có**PID riêng**                                                                                                                                 | Cùng PID với tiến trình cha                                           |
| Lập lịch hệ điều hành | Tiến trình được lập lịch**độc lập**                                                                                                     | Luồng được lập lịch trong**cùng tiến trình**               |

→ Khi tạo luồng mới, nó  **chia sẻ** :

* Các biến toàn cục (`global variables`)
* Các file descriptor đang mở
* Các trình xử lý tín hiệu (`signal handlers`)
* Trạng thái thư mục hiện tại [Current Directory State](#-current-directory-state-là-gì)
* Không gian bộ nhớ: code segment, data segment, heap

---

### 📦 POSIX Threads (pthreads)

* Khái niệm luồng đã tồn tại từ lâu, nhưng cách triển khai  **khác nhau giữa các hệ thống UNIX** .
* Tiêu chuẩn **POSIX 1003.1c** cung cấp API chuẩn cho việc lập trình luồng → đảm bảo tính **portable** giữa các hệ điều hành tuân thủ POSIX như Linux.
* Trên Linux, triển khai hiện đại nhất là **NPTL (Native POSIX Thread Library)**

  → hiệu suất cao, khác biệt so với LinuxThreads cũ.

---

## ✅❌ **1.2. Ưu điểm và Hạn chế của Luồng**

### ✅ Ưu điểm (Advantages)

* **Hiệu suất (Performance):**

  → Luồng có thể chạy song song trên **đa lõi (multi-core CPUs)**

  → Trên hệ thống đơn lõi, kernel xen kẽ thực thi (`time-slicing`) → tạo ảo giác đồng thời.
* **Chia sẻ tài nguyên (Resource Sharing):**

  → Luồng cùng tiến trình chia sẻ memory → giao tiếp giữa luồng nhanh hơn nhiều so với IPC giữa tiến trình riêng biệt.
* **Chi phí thấp (Lower Overhead):**

  → Tạo & chuyển đổi ngữ cảnh giữa luồng nhẹ hơn nhiều so với tiến trình.
* **Phản hồi nhanh (Responsiveness):**

  → Có thể chạy tác vụ nặng trong luồng riêng → giữ `main thread` luôn mượt mà.
* **Phù hợp với ứng dụng dữ liệu liên kết chặt chẽ:**

  → Ví dụ: database server đa luồng phục vụ nhiều client cần truy cập chung.

---

### ❌ Hạn chế (Drawbacks)

* **Đồng bộ hóa phức tạp:**

  → Cần dùng cơ chế như `mutex`, `semaphore`, `condition variables` để bảo vệ dữ liệu tránh  **race condition** .
* **Deadlock:**

  → Khi các luồng **chờ tài nguyên** mà luồng khác đang giữ → có thể khiến hệ thống treo.
* **Lỗi khó tìm:**

  → Lỗi liên quan đến thời gian và đồng bộ hóa  **khó tái hiện và debug** .
* **Khó kiểm soát tài nguyên:**

  → Luồng nhẹ hơn tiến trình, nhưng nếu tạo quá nhiều vẫn có thể gây quá tải hệ thống.

---

## 🚀 **1.3. Thực thi Đồng thời (Simultaneous Execution)**

### 📖 Lý thuyết

* Khi tạo nhiều luồng, OS sẽ **lập lịch** để chúng thực thi đồng thời.
* **Trên CPU đa lõi:** Luồng chạy  **song song thực sự** .
* **Trên CPU đơn lõi:** Kernel dùng kỹ thuật **chia sẻ thời gian (time-slicing)** → nhanh chóng xen kẽ các luồng → tạo ảo giác đồng thời.

---

### ❌ Vấn đề Busy-Wait (Thăm dò bận rộn)

* Là kỹ thuật đồng bộ hoá  **không hiệu quả** , ví dụ:

```cpp
while (condition) {
    // kiểm tra liên tục mà không sleep
}
```

* Điều này **tiêu tốn CPU** không cần thiết.
* Thay vào đó: nên dùng **blocking sync** như `semaphore`, `mutex`, hoặc cho luồng **ngủ (sleep)`.

---

### 💻 Ví dụ (`thread2.c` / `thread2.cpp`)

* Hai luồng thay phiên in `1` và `2` bằng biến toàn cục `run_now`.
* Sử dụng busy-wait:

```cpp
if (run_now == X) { ... }
else { sleep(1); }
```

→ Code minh họa cách đồng bộ đơn giản nhưng không tối ưu.

---

### 🔧 Liên hệ Embedded Linux

* Trong hệ nhúng, **thực thi đồng thời** là lý do chính để dùng luồng.
* Cần  **tránh busy-wait** , dùng sync hiệu quả để tiết kiệm tài nguyên CPU và năng lượng.

---

## 🤔 **Câu hỏi Tự đánh giá Module 1**

1. Giải thích sự khác biệt cơ bản giữa một **tiến trình (process)** và một **luồng (thread)** về mặt tài nguyên mà chúng sử dụng và chia sẻ.
2. Nêu ít nhất ba ưu điểm chính của việc sử dụng luồng so với tiến trình khi thiết kế một ứng dụng.
3. Nêu ít nhất hai thách thức hoặc hạn chế chính khi viết chương trình đa luồng. Điều gì là vấn đề khó tìm lỗi nhất trong số đó?
4. "POSIX Threads" là gì và tại sao tiêu chuẩn này lại quan trọng đối với lập trình luồng?
5. Giải thích khái niệm "Busy-Wait" trong lập trình luồng. Tại sao nó được coi là một phương pháp đồng bộ hóa kém hiệu quả?

---

## ✍️ **Bài tập Thực hành Module 1**

### 1. **Chương trình "Process vs. Thread Overhead"**

* Viết hai chương trình C++ riêng biệt:

#### `process_overhead.cpp`

Trong `main()`, tạo một vòng lặp chạy 100 lần. Trong mỗi lần lặp:

* `fork()` một tiến trình con
* Tiến trình con in ra PID của nó và thoát ngay (`exit(0)`)
* Tiến trình cha `wait()` cho con kết thúc

#### `thread_overhead.cpp`

Trong `main()`, tạo một vòng lặp chạy 100 lần. Trong mỗi lần lặp:

* Tạo một luồng con với `pthread_create()`
* Luồng con in ra ID của nó và thoát với `pthread_exit(NULL)`
* Luồng cha `pthread_join()` chờ luồng con kết thúc

#### Biên dịch

```bash
g++ process_overhead.cpp -o process_overhead
g++ thread_overhead.cpp -o thread_overhead -pthread
```

#### Chạy thử

```bash
time ./process_overhead
time ./thread_overhead
```

→ So sánh thời gian thực thi và giải thích tại sao một chương trình lại nhanh hơn đáng kể so với chương trình kia.

---

### 2. **Chương trình "Busy-Wait Demo"**

* Viết chương trình C++ `busy_wait_demo.cpp` mô phỏng ví dụ `thread2.c`.

Có hai luồng và một biến toàn cục `run_now`:

* **`main` thread:**
  * Nếu `run_now == 1`: in `'1'`, đặt `run_now = 2`
  * Nếu không: `sleep(1)`
* **`thread_function`:**
  * Nếu `run_now == 2`: in `'2'`, đặt `run_now = 1`
  * Nếu không: `sleep(1)`

#### Thử thách

* Xóa tất cả các lệnh `sleep(1)`
* Chạy lại chương trình
* Dùng `top` hoặc `htop` để quan sát mức sử dụng CPU

→ Giải thích vì sao chương trình lại tiêu tốn CPU nhiều như vậy khi không có `sleep()`

---

## 📂 **Current Directory State là gì?**

* Mỗi **tiến trình hoặc luồng** trong hệ điều hành có thể đang "đứng" ở một vị trí cụ thể trong hệ thống file — đó chính là **thư mục hiện tại** (CWD).
* Khi Anh gọi các hàm thao tác file, như `open("data.txt")`, hệ thống sẽ  **tìm file này tương đối với thư mục hiện tại** , nếu không cung cấp đường dẫn tuyệt đối.

---

## 🔧 Ví dụ minh họa

Giả sử thư mục hiện tại là `/home/anh/project`, thì đoạn sau:

```cpp
int fd = open("log.txt", O_WRONLY);
```

→ sẽ mở `/home/anh/project/log.txt`, chứ không phải `/log.txt`.

---

## ✅ Luồng và thư mục hiện tại?

* Trong `fork()`: tiến trình con **sao chép** trạng thái thư mục hiện tại từ cha → con đứng cùng chỗ.
* Trong `pthread_create()`:  **tất cả luồng cùng một tiến trình chia sẻ cùng thư mục hiện tại** .

🔄 Nếu một luồng gọi `chdir("/tmp")` để đổi thư mục, **các luồng khác cũng bị ảnh hưởng** → vì chúng thuộc cùng tiến trình.

---

## 📦 Các hàm liên quan

| Hàm         | Mô tả                                            |
| ------------ | -------------------------------------------------- |
| `getcwd()` | Lấy thư mục hiện tại của tiến trình/luồng |
| `chdir()`  | Đổi thư mục hiện tại                         |

---

## ⚠️ Thực chiến trong Embedded Linux

* Khi viết  **daemon hoặc luồng ghi log** , nên dùng đường dẫn tuyệt đối nếu có nhiều luồng xử lý → tránh bị ảnh hưởng bởi việc đổi CWD ở nơi khác.
* Một số thư viện hoặc package vô tình gọi `chdir()` sẽ gây lỗi khó debug nếu không kiểm soát tốt.

---
