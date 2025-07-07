
-----

### **Giáo trình: System V IPC (Semaphores, Shared Memory, Message Queues) ⚙️✉️🧠**

**Mục tiêu của Giáo trình 🎯**

Sau khi hoàn thành giáo trình này, bạn sẽ có thể:

  * Hiểu khái niệm tổng thể về System V IPC và tầm quan trọng của nó trong giao tiếp giữa các tiến trình.
  * Nắm vững cách sử dụng Semaphores (đặc biệt là System V Semaphores) để quản lý quyền truy cập tài nguyên giữa các tiến trình.
  * Hiểu cách thức hoạt động và sử dụng Shared Memory để chia sẻ dữ liệu hiệu quả giữa các tiến trình.
  * Nắm vững cách sử dụng Message Queues để truyền dữ liệu có cấu trúc giữa các tiến trình một cách tiện lợi.
  * Phân biệt và lựa chọn cơ chế IPC phù hợp cho các tình huống khác nhau.
  * Sử dụng các công cụ dòng lệnh để giám sát và dọn dẹp các tài nguyên IPC còn sót lại.
  * Liên hệ các kiến thức này với Windows, RTOS và môi trường nhúng.

-----

### **Cấu trúc Giáo trình 📚**

Giáo trình này sẽ được chia thành các Modules nhỏ để dễ dàng theo dõi và tiếp thu:

  * **Module 1: Giới thiệu System V IPC và Semaphores**
  * **Module 2: Shared Memory (Bộ nhớ chia sẻ)**
  * **Module 3: Message Queues (Hàng đợi tin nhắn)**
  * **Module 4: Công cụ IPC và Tổng hợp & Ứng dụng**

Mỗi Module sẽ bao gồm:

  * **Lý thuyết chi tiết:** Giải thích các khái niệm, hàm, và cơ chế.
  * **Ví dụ Code (C++):** Minh họa cách sử dụng các hàm (chỉ dùng `std::cout`/`std::cerr`).
  * **Liên hệ với Embedded Linux, Windows, RTOS:** Giải thích tầm quan trọng và ứng dụng trong các môi trường khác nhau.
  * **Hình ảnh/Biểu đồ:** Minh họa để dễ hiểu hơn.
  * **Câu hỏi Tự đánh giá:** Giúp bạn kiểm tra mức độ hiểu bài.
  * **Bài tập Thực hành:** Các bài tập coding để bạn áp dụng kiến thức.

Bây giờ, chúng ta sẽ bắt đầu với **Module 1: Giới thiệu System V IPC và Semaphores** và em sẽ chờ xác nhận của anh trước khi chuyển sang Module tiếp theo.

-----

### **Module 1: Giới thiệu System V IPC và Semaphores 🚦**

Module này sẽ giới thiệu tổng quan về System V IPC và đi sâu vào một trong những cơ chế quan trọng nhất của nó: Semaphores, dùng để đồng bộ hóa quyền truy cập tài nguyên giữa các tiến trình.

#### **1.1. System V IPC là gì? 🧠✉️🔗**

  * **Lý thuyết:**
      * **IPC (Inter-Process Communication - Giao tiếp Liên Tiến trình):** Là tập hợp các cơ chế cho phép các tiến trình trao đổi thông tin và đồng bộ hóa hoạt động với nhau. Anh đã thấy `pipe()` và `popen()` là các ví dụ về IPC đơn giản.
      * **System V IPC:** Là một bộ các cơ chế IPC mạnh mẽ và đa năng, được giới thiệu lần đầu trong phiên bản UNIX System V Release 2. Chúng có giao diện lập trình tương tự nhau và thường được gọi chung là "System V IPC".
      * **Các cơ chế System V IPC:**
        1.  **Semaphores (Semaphore):** Dùng để quản lý quyền truy cập vào các tài nguyên chung (đặc biệt là các vùng găng - critical sections).
        2.  **Shared Memory (Bộ nhớ chia sẻ):** Cho phép các tiến trình truy cập cùng một vùng bộ nhớ logic, là cách hiệu quả nhất để chia sẻ lượng lớn dữ liệu.
        3.  **Message Queues (Hàng đợi tin nhắn):** Cung cấp cách dễ dàng để truyền các khối dữ liệu có cấu trúc (tin nhắn) giữa các tiến trình.
      * **Đặc điểm chung của System V IPC:**
          * Sử dụng **khóa (key\_t)** để định danh một tài nguyên IPC (giống như tên file hoặc số hiệu cổng). Đây là cách để các tiến trình không liên quan có thể tìm thấy và truy cập cùng một tài nguyên IPC.
          * Sử dụng **ID (identifier)** sau khi tài nguyên IPC được tạo hoặc lấy (`semid`, `shmid`, `msqid`). Đây là giá trị mà tiến trình sử dụng để thao tác trực tiếp với tài nguyên đó (giống như File Descriptor).
          * Tài nguyên IPC tồn tại **độc lập** với tiến trình đã tạo ra nó. Điều này có nghĩa là nếu tiến trình tạo ra tài nguyên đó kết thúc, tài nguyên (ví dụ: semaphore, shared memory segment, message queue) vẫn tồn tại trong hệ thống cho đến khi được xóa một cách tường minh (`ipcrm` hoặc hàm điều khiển) hoặc khi hệ thống khởi động lại. Đây là một điểm khác biệt lớn so với unnamed pipes hay các đối tượng tạm thời khác.
  * **Liên hệ Embedded Linux:** System V IPC rất mạnh mẽ trên thiết bị nhúng cho các daemon giao tiếp với nhau hoặc với các ứng dụng khác mà không cần cấu hình mạng phức tạp. Khả năng tồn tại độc lập giúp các dịch vụ ổn định hơn.

#### **1.2. Semaphores (System V) là gì? 🚦**

  * **Lý thuyết:**

      * Semaphore là một biến nguyên (integer variable) được dùng để đồng bộ hóa quyền truy cập vào một tài nguyên hoặc vùng găng (critical section).
      * **Khác biệt quan trọng:** System V Semaphores **khác** với các semaphore dùng với threads (Pthreads semaphores - `sem_init`, `sem_wait`, `sem_post`) mà anh đã học. System V Semaphores được thiết kế để đồng bộ hóa giữa các **tiến trình (processes)** và có giao diện phức tạp hơn, hoạt động trên các **mảng semaphores** (ngay cả khi anh chỉ dùng một semaphore, nó vẫn là một mảng 1 phần tử).
      * **Mục đích:** Đảm bảo chỉ một tiến trình (hoặc một số lượng giới hạn tiến trình) được phép truy cập một tài nguyên chia sẻ tại một thời điểm.
      * **Thao tác cơ bản (ý tưởng của Dijkstra):**
          * **P(sv)** (Passeren - to pass, "down"): Giảm giá trị của semaphore (`sv`). Nếu `sv` là 0, tiến trình bị chặn cho đến khi `sv` lớn hơn 0.
          * **V(sv)** (Vrijgeven - to release, "up"): Tăng giá trị của semaphore (`sv`). Nếu có tiến trình nào đang bị chặn chờ `sv`, một trong số chúng sẽ được đánh thức.
      * **Binary Semaphore:** Chỉ nhận giá trị 0 hoặc 1 (phổ biến nhất cho độc quyền tương hỗ).
      * **General Semaphore (Counting Semaphore):** Có thể nhận nhiều giá trị dương, dùng để kiểm soát số lượng tài nguyên có sẵn.
      * **Atomic Operations:** Các thao tác P và V là **nguyên tử (atomic)**, nghĩa là chúng không thể bị gián đoạn. Điều này là cốt lõi để tránh race condition khi kiểm tra và thay đổi giá trị semaphore.

  * **Minh họa (Semaphore làm cổng kiểm soát):**

    ```mermaid
    graph TD
        P_A[Process A] --> P_OP_A(P(Semaphore))
        P_OP_A --> Critical_Section(Vùng Găng)
        Critical_Section --> V_OP_A(V(Semaphore))
        V_OP_A --> Non_Critical_Section_A(Vùng không Găng A)
        Non_Critical_Section_A --> Loop_A(Tiếp tục A)
        Loop_A --> P_OP_A

        P_B[Process B] --> P_OP_B(P(Semaphore))
        P_OP_B --> Critical_Section
        Critical_Section --> V_OP_B(V(Semaphore))
        V_OP_B --> Non_Critical_Section_B(Vùng không Găng B)
        Non_Critical_Section_B --> Loop_B(Tiếp tục B)
        Loop_B --> P_OP_B

        subgraph Semaphore Gate
            P_OP_A
            P_OP_B
        end
        
        style Critical_Section fill:#f9f,stroke:#333,stroke-width:2px;
        style Semaphore_Gate fill:#ddd,stroke:#333,stroke-width:2px;
    ```

      * Hình trên minh họa cách `P()` hoạt động như một "cổng vào" vùng găng, và `V()` là "cổng ra", đảm bảo chỉ một tiến trình được vào vùng găng tại một thời điểm.

  * **So sánh với Khóa File (`O_EXCL` với `open()`):**

      * `open(file, O_CREAT | O_EXCL)` có thể dùng như một semaphore nhị phân đơn giản để lấy token.
      * Tuy nhiên, System V Semaphores hiệu quả hơn nhiều vì:
          * **Không busy-wait:** Tiến trình bị chặn sẽ không tiêu tốn CPU.
          * **Atomic:** Được Kernel đảm bảo tính nguyên tử cho mọi thao tác.
          * **`SEM_UNDO`:** Khả năng tự động giải phóng nếu tiến trình crash.
          * **Linh hoạt:** Có thể hoạt động trên mảng semaphores (counting semaphores).

#### **1.3. Các Hàm Semaphore của Linux (System V) 🔑**

Giao diện System V Semaphore phức tạp hơn Pthreads Semaphore vì nó hoạt động trên **mảng semaphores** và cung cấp nhiều kiểm soát hơn. Anh cần bao gồm các header file: `<sys/sem.h>`, `<sys/types.h>`, `<sys/ipc.h>`.

  * **`semget()`: Lấy Semaphore ID 🆔**

      * **Mục đích:** Tạo một semaphore mới hoặc lấy ID của một semaphore hiện có.
      * **Syntax:**
        ```cpp
        #include <sys/sem.h>   // Cho semget, semctl, semop, sembuf, semun
        #include <sys/types.h> // Cho key_t, pid_t
        #include <sys/ipc.h>   // Cho IPC_PRIVATE, IPC_CREAT, IPC_RMID (cờ IPC chung)
        // int semget(key_t key, int num_sems, int sem_flags);
        ```
      * **`key`**: Một giá trị nguyên (`key_t`) dùng để định danh semaphore. Các tiến trình không liên quan có thể truy cập cùng semaphore nếu chúng sử dụng cùng `key`.
          * `IPC_PRIVATE`: Key đặc biệt để tạo semaphore chỉ tiến trình tạo mới có thể truy cập (trên một số hệ thống Linux có thể không hoàn toàn riêng tư).
          * `ftok()`: Hàm này thường được dùng để tạo một `key_t` duy nhất từ một đường dẫn file và một số nguyên.
      * **`num_sems`**: Số lượng semaphores trong mảng semaphores cần tạo/lấy. Thường là `1` cho một semaphore đơn.
      * **`sem_flags`**: Một tập hợp các cờ bitwise OR (`|`) với quyền hạn (permissions):
          * **Quyền hạn:** 9 bit thấp nhất tương tự quyền file (ví dụ: `0666` cho r/w bởi owner, group, others).
          * `IPC_CREAT`: Tạo semaphore mới nếu chưa tồn tại.
          * `IPC_EXCL`: Dùng với `IPC_CREAT`. Đảm bảo semaphore là duy nhất mới được tạo. Nếu nó đã tồn tại, `semget()` sẽ thất bại (`EEXIST`).
      * **Giá trị trả về:** ID của semaphore (`int`) nếu thành công (sử dụng trong các hàm `semop`, `semctl`), `-1` nếu thất bại.

  * **`semop()`: Thực hiện Thao tác P và V (và các thay đổi khác) ⬅️➡️**

      * **Mục đích:** Thực hiện các thao tác P (wait/down) hoặc V (signal/up) trên một hoặc nhiều semaphores trong một mảng semaphores.
      * **Syntax:**
        ```cpp
        #include <sys/sem.h>
        // int semop(int sem_id, struct sembuf *sem_ops, size_t num_sem_ops);
        ```
      * **`sem_id`**: ID của semaphore, trả về từ `semget()`.
      * **`sem_ops`**: Con trỏ tới một mảng các cấu trúc `struct sembuf`. Mỗi `sembuf` mô tả một thao tác trên một semaphore cụ thể.
      * **`num_sem_ops`**: Số lượng thao tác trong mảng `sem_ops`.
      * **`struct sembuf`:**
        ```cpp
        struct sembuf {
            short sem_num; // Chỉ số semaphore trong mảng (thường là 0 cho semaphore đơn)
            short sem_op;  // Thao tác: -1 cho P(), +1 cho V(). Có thể là số nguyên bất kỳ.
            short sem_flg; // Cờ thao tác
        };
        ```
          * `sem_flg`:
              * **`SEM_UNDO`**: **Rất quan trọng\!** Nếu tiến trình chấm dứt (crash hoặc thoát) mà không giải phóng semaphore mà nó đã thay đổi, Kernel sẽ tự động "hoàn tác" các thay đổi đó. Điều này đảm bảo semaphore được trả về trạng thái hợp lệ và tránh bị "kẹt" (deadlock do semaphore bị giữ). Đây là hành vi nên dùng.
              * `IPC_NOWAIT`: Nếu thao tác không thể thực hiện ngay lập tức (ví dụ: `sem_op = -1` và semaphore bằng 0), `semop()` sẽ trả về ngay lập tức với `errno = EAGAIN` thay vì chặn.
      * **Tính nguyên tử của `semop`:** Nếu `sem_ops` chứa nhiều thao tác trên các semaphores khác nhau, tất cả các thao tác đó sẽ được thực hiện **nguyên tử** (tất cả cùng thành công hoặc không cái nào thành công), tránh race condition phức tạp.
      * **Giá trị trả về:** `0` nếu thành công, `-1` nếu thất bại.

  * **`semctl()`: Điều khiển Semaphore Trực tiếp 🕹️**

      * **Mục đích:** Thực hiện các thao tác điều khiển trực tiếp trên semaphore (ví dụ: khởi tạo giá trị, lấy thông tin, hoặc xóa semaphore).
      * **Syntax:**
        ```cpp
        #include <sys/sem.h>
        // int semctl(int sem_id, int sem_num, int command, ... /* union semun arg */);
        ```
      * **`sem_id`**: ID của semaphore.
      * **`sem_num`**: Chỉ số semaphore trong mảng (thường là `0`).
      * **`command`**: Lệnh cần thực hiện:
          * **`SETVAL`**: **Khởi tạo giá trị** cho một semaphore. Đối số thứ tư là `union semun` với trường `val` chứa giá trị ban đầu. Đây là bước **bắt buộc** để semaphore hoạt động đúng.
          * **`IPC_RMID`**: **Xóa semaphore** khỏi hệ thống.
          * `GETVAL`: Lấy giá trị hiện tại của semaphore.
          * `IPC_STAT`: Lấy thông tin trạng thái của semaphore vào cấu trúc `struct semid_ds`.
      * **`union semun`**: Một union đặc biệt được dùng để truyền các đối số cho `semctl()`. Rất nhiều phiên bản Linux yêu cầu bạn **tự định nghĩa** `union semun` trước khi sử dụng.
        ```cpp
        // Cần định nghĩa union semun trước khi dùng semctl() trên nhiều hệ thống
        // Nếu trình biên dịch báo lỗi 'semun' undeclared, bỏ comment đoạn này.
        /*
        union semun {
            int val;                // Value for SETVAL
            struct semid_ds *buf;   // Buffer for IPC_STAT, IPC_SET
            unsigned short *array;  // Array for GETALL, SETALL
            // struct seminfo *__buf; // Buffer for IPC_INFO (Linux-specific)
        };
        */
        ```

  * **Liên hệ Embedded Linux:**

      * System V Semaphores là công cụ mạnh mẽ để đồng bộ hóa quyền truy cập vào các tài nguyên phần cứng chia sẻ hoặc các vùng dữ liệu chung giữa các daemon chạy độc lập trên thiết bị nhúng.
      * Việc sử dụng `SEM_UNDO` là cực kỳ quan trọng để đảm bảo tính bền vững của hệ thống.

#### **1.4. Liên hệ với Windows và RTOS 🤝**

  * **Độc lập Kiến thức:** Các kiến thức về System V IPC là **độc lập** với các Module khác về Processes, Signals, Threads. Mặc dù chúng thường được sử dụng cùng nhau, nhưng mỗi phần có API và mục đích riêng.

  * **Windows:**

      * Windows không có System V IPC (`semget`, `shmget`, `msgget`) theo chuẩn POSIX.
      * Để đồng bộ hóa giữa các tiến trình, Windows sử dụng các đối tượng như **Mutexes (tên), Semaphores (tên), Events, Waitable Timers**. Các đối tượng này có thể có tên và tồn tại độc lập với tiến trình tạo ra chúng.
      * **Named Mutexes/Semaphores:** Cho phép đồng bộ hóa giữa các tiến trình không liên quan.
      * **Memory-Mapped Files:** Được sử dụng để chia sẻ bộ nhớ giữa các tiến trình (tương tự `mmap` trong Linux, nhưng `shmget` của System V là khác).
      * **Message Queues:** Windows cũng có cơ chế message queue thông qua các API khác (ví dụ: Windows Messaging System cho GUI, hoặc các cơ chế RPC/COM cho giao tiếp phức tạp).

  * **RTOS (Real-Time Operating Systems) như FreeRTOS:**

      * RTOS thường **không có** khái niệm System V IPC.
      * Để đồng bộ hóa và chia sẻ dữ liệu giữa các **tasks** (tương đương luồng trong RTOS), RTOS sử dụng các cơ chế nội bộ được tối ưu hóa cho thời gian thực:
          * **Semaphores (Binary/Counting):** Rất phổ biến, hoạt động tương tự về mặt khái niệm như semaphores (Pthreads), nhưng với API của RTOS (`xSemaphoreCreateMutex`, `xSemaphoreTake`, `xSemaphoreGive`).
          * **Message Queues:** (Giống như System V Message Queues về chức năng, nhưng API riêng của RTOS). Task gửi/nhận các gói tin có cấu trúc.
          * **Shared Memory:** Thường được quản lý đơn giản hơn (ví dụ: chỉ cần khai báo một biến toàn cục và bảo vệ bằng mutex/semaphore của RTOS). Không có các API `shmget`/`shmat` phức tạp.
      * RTOS ưu tiên tính xác định (determinism), do đó các cơ chế IPC của nó được thiết kế để có hành vi dự đoán được hơn so với System V IPC của Linux.

#### **1.5. Ví dụ (C++): `sem_example.cpp` - Semaphore System V Đơn giản**

Chương trình này mô phỏng hai (hoặc nhiều hơn) tiến trình cố gắng truy cập một vùng găng được bảo vệ bởi một System V Semaphore nhị phân.

```cpp
#include <iostream>  // For std::cout, std::cerr
#include <string>    // For std::string, std::to_string
#include <fcntl.h>   // For O_RDWR, O_CREAT (không dùng trực tiếp bởi sem functions)
#include <sys/stat.h> // For mode_t (không dùng trực tiếp bởi sem functions)
#include <sys/sem.h> // For semget, semctl, semop, sembuf, SETVAL, IPC_RMID
#include <sys/types.h> // For key_t, pid_t (thường được include bởi các header trên)
#include <sys/ipc.h> // For IPC_CREAT
#include <unistd.h>  // For getpid, sleep
#include <cstdlib>   // For EXIT_SUCCESS, EXIT_FAILURE, srand, rand
#include <cstring>   // For strerror (dùng cho std::perror)
#include <cerrno>    // For errno
#include <ctime>     // For time (for srand)

// ==========================================================
// CẦN THIẾT: Định nghĩa union semun trên nhiều hệ thống Linux
// ==========================================================
// Nếu trình biên dịch của anh báo lỗi 'semun' undeclared, hãy bỏ comment đoạn code dưới đây.
// Với Glibc hiện đại, việc này thường không còn cần thiết vì nó đã được định nghĩa trong <sys/sem.h>
/*
union semun {
    int val;                // Value for SETVAL
    struct semid_ds *buf;   // Buffer for IPC_STAT, IPC_SET
    unsigned short *array;  // Array for GETALL, SETALL
    // struct seminfo *__buf; // Buffer for IPC_INFO (Linux-specific)
};
*/

static int sem_id; // ID của semaphore, dùng chung cho các hàm trợ giúp

// Hàm trợ giúp: Khởi tạo giá trị semaphore
static int set_semvalue() {
    union semun sem_union; // Cần định nghĩa union semun nếu trình biên dịch báo lỗi
    sem_union.val = 1; // Đặt giá trị ban đầu là 1 (cho binary semaphore)
    std::cout << "INFO: Initializing semaphore with value 1." << std::endl;
    if (semctl(sem_id, 0, SETVAL, sem_union) == -1) {
        std::cerr << "ERROR: set_semvalue failed: " << std::strerror(errno) << std::endl;
        return 0; // Thất bại
    }
    std::cout << "SUCCESS: Semaphore value set to 1." << std::endl;
    return 1; // Thành công
}

// Hàm trợ giúp: Xóa semaphore khỏi hệ thống
static void del_semvalue() {
    union semun sem_union; // Cần định nghĩa union semun nếu trình biên dịch báo lỗi
    std::cout << "INFO: Deleting semaphore..." << std::endl;
    if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1) {
        std::cerr << "ERROR: del_semvalue failed: " << std::strerror(errno) << std::endl;
    } else {
        std::cout << "SUCCESS: Semaphore deleted successfully." << std::endl;
    }
}

// Hàm trợ giúp: Thao tác P() - Giảm semaphore (Wait)
static int semaphore_p() {
    struct sembuf sem_b;
    sem_b.sem_num = 0;   // Chỉ số semaphore trong mảng (chỉ có 1 semaphore)
    sem_b.sem_op = -1;   // Thao tác P() - giảm 1 (wait)
    sem_b.sem_flg = SEM_UNDO; // Đảm bảo hoàn tác nếu tiến trình crash

    std::cout << "INFO: Process " << getpid() << ": Attempting to acquire semaphore (P())." << std::endl;
    // semop() thực hiện thao tác. Nếu semaphore bằng 0, tiến trình sẽ chặn.
    if (semop(sem_id, &sem_b, 1) == -1) {
        std::cerr << "ERROR: Process " << getpid() << ": semaphore_p failed: " << std::strerror(errno) << std::endl;
        return 0; // Thất bại
    }
    std::cout << "SUCCESS: Process " << getpid() << ": Acquired semaphore." << std::endl;
    return 1; // Thành công
}

// Hàm trợ giúp: Thao tác V() - Tăng semaphore (Signal)
static int semaphore_v() {
    struct sembuf sem_b;
    sem_b.sem_num = 0;   // Chỉ số semaphore trong mảng
    sem_b.sem_op = 1;    // Thao tác V() - tăng 1 (signal)
    sem_b.sem_flg = SEM_UNDO; // Đảm bảo hoàn tác

    std::cout << "INFO: Process " << getpid() << ": Releasing semaphore (V())." << std::endl;
    if (semop(sem_id, &sem_b, 1) == -1) {
        std::cerr << "ERROR: Process " << getpid() << ": semaphore_v failed: " << std::strerror(errno) << std::endl;
        return 0; // Thất bại
    }
    std::cout << "SUCCESS: Process " << getpid() << ": Released semaphore." << std::endl;
    return 1; // Thành công
}

int main(int argc, char *argv[]) {
    pid_t my_pid = getpid();
    int i;
    int pause_time;
    char op_char = 'O'; // Ký tự in ra bởi các tiến trình bình thường ('O')

    std::srand(static_cast<unsigned int>(my_pid)); // Khởi tạo seed cho rand() dựa trên PID

    std::cout << "INFO: Process " << std::to_string(my_pid) << ": Starting Semaphore System V demonstration." << std::endl;

    // 1. Lấy hoặc Tạo Semaphore
    // Sử dụng key 1234. Tạo nếu không tồn tại (IPC_CREAT). Quyền 0666 (rw-rw-rw-).
    sem_id = semget(static_cast<key_t>(1234), 1, 0666 | IPC_CREAT);
    if (sem_id == -1) {
        std::cerr << "CRITICAL: Process " << std::to_string(my_pid) << ": semget failed: " << std::strerror(errno) << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "SUCCESS: Process " << std::to_string(my_pid) << ": Semaphore ID obtained: " << sem_id << std::endl;

    // 2. Nếu là tiến trình đầu tiên (có đối số), khởi tạo semaphore và là "X" writer
    // Điều này đảm bảo chỉ một tiến trình khởi tạo giá trị ban đầu.
    if (argc > 1) {
        std::cout << "INFO: Process " << std::to_string(my_pid) << ": (First process) Initializing semaphore value to 1." << std::endl;
        if (!set_semvalue()) { // Khởi tạo giá trị semaphore
            std::cerr << "CRITICAL: Process " << std::to_string(my_pid) << ": Failed to initialize semaphore." << std::endl;
            return EXIT_FAILURE;
        }
        op_char = 'X'; // Đổi ký tự in ra thành 'X'
        std::cout << "INFO: Process " << std::to_string(my_pid) << ": Sleeping 2s to allow other processes to start and queue up." << std::endl;
        sleep(2); // Cho phép các tiến trình khác có thời gian khởi động
    }

    // 3. Vòng lặp chính: Thực hiện 10 lần vào/ra vùng găng
    for (i = 0; i < 10; ++i) {
        std::cout << "INFO: Process " << std::to_string(my_pid) << ": Iteration " << i + 1 << "/10. Attempting to enter critical section (P())." << std::endl;
        if (!semaphore_p()) { // Gọi P() - chặn nếu semaphore bằng 0
            std::cerr << "ERROR: Process " << std::to_string(my_pid) << ": Failed to acquire semaphore. Exiting loop." << std::endl;
            break;
        }

        // --- BẮT ĐẦU VÙNG GĂNG (CRITICAL SECTION) ---
        printf("%c", op_char); fflush(stdout); // In ký tự (dùng printf/fflush để flush ngay lập tức)
        pause_time = rand() % 3; // Ngủ ngẫu nhiên 0, 1, hoặc 2 giây
        sleep(pause_time);
        printf("%c", op_char); fflush(stdout); // In ký tự lần 2
        // --- KẾT THÚC VÙNG GĂNG ---

        std::cout << "INFO: Process " << std::to_string(my_pid) << ": Exited critical section. Releasing semaphore (V())." << std::endl;
        if (!semaphore_v()) { // Gọi V() - tăng semaphore
            std::cerr << "ERROR: Process " << std::to_string(my_pid) << ": Failed to release semaphore. Exiting loop." << std::endl;
            break;
        }

        pause_time = rand() % 2; // Ngủ ngẫu nhiên 0 hoặc 1 giây (vùng không găng)
        sleep(pause_time);
    }
    
    printf("\nProcess %d - finished.\n", my_pid); // In thông báo kết thúc

    // 4. Tiến trình đầu tiên (có đối số) chịu trách nhiệm xóa semaphore
    // Đảm bảo semaphore được xóa khi không còn ai dùng, tránh để lại rác IPC.
    if (argc > 1) {
        std::cout << "INFO: Process " << std::to_string(my_pid) << ": (First process) Waiting for other processes to finish (10s) before deleting semaphore." << std::endl;
        sleep(10); // Cho phép các tiến trình khác hoàn thành
        del_semvalue(); // Xóa semaphore
    }

    return EXIT_SUCCESS;
}
```

-----

### **Cách Biên dịch và Chạy:**

1.  **Biên dịch:**
    ```bash
    g++ sem_example.cpp -o sem_example
    ```
2.  **Cách Chạy và Kiểm tra:**
      * **Mở nhiều terminal** (ít nhất 2).
      * **Terminal 1 (Người tạo & xóa semaphore):**
        ```bash
        ./sem_example 1 & # Chạy ở chế độ nền, có đối số '1'
        # Hoặc chỉ './sem_example 1' nếu muốn xem trực tiếp.
        # Nó sẽ in ra "XX..."
        ```
      * **Terminal 2 (Các người dùng semaphore khác):**
        ```bash
        ./sem_example # Chạy ở chế độ tiền cảnh, không có đối số
        # Nó sẽ in ra "OO..."
        ```
      * **Terminal 3 (hoặc các terminal khác):** Chạy thêm `./sem_example` để xem thêm tiến trình tranh giành.

<!-- end list -->

  * **Phân tích Output:**
      * Bạn sẽ thấy các cặp ký tự (`XX` hoặc `OO`) xuất hiện tuần tự, không bị lẫn lộn (ví dụ: không bao giờ có `XO` hoặc `OX` nếu nó là vùng găng thực sự).
      * Điều này chứng tỏ semaphore đang hoạt động đúng, chỉ cho phép một tiến trình vào vùng găng tại một thời điểm.
      * Tiến trình đầu tiên (`./sem_example 1`) sẽ ngủ lâu hơn ở cuối để đảm bảo các tiến trình khác có thời gian hoàn thành trước khi semaphore bị xóa.
      * Nếu bạn quên chạy tiến trình đầu tiên với đối số, các tiến trình sau sẽ báo lỗi "semget failed: No such file or directory" (semaphore không tồn tại).
      * Nếu một tiến trình bị crash trong vùng găng, `SEM_UNDO` sẽ đảm bảo semaphore được giải phóng. Bạn có thể thử `kill -9` một tiến trình đang chạy `sem_example` giữa cặp ký tự `XX` hoặc `OO`.

-----


### **Câu hỏi Tự đánh giá Module 1 🤔**

1.  Giải thích sự khác biệt giữa System V IPC Semaphores và Pthreads Semaphores. Chúng được thiết kế để đồng bộ hóa giữa các thực thể nào?
2.  Semaphore là gì? Nêu hai thao tác cơ bản trên semaphore và ý nghĩa của chúng.
3.  Tại sao các thao tác trên semaphore phải là "atomic" (nguyên tử)?
4.  Hàm `semget()` được dùng để làm gì? Giải thích vai trò của `key` và `sem_flags` (đặc biệt là `IPC_CREAT` và `IPC_EXCL`).
5.  Giải thích cách `semop()` thực hiện thao tác P và V. Tại sao `sem_op` có thể là `-1` hoặc `+1`?
6.  `SEM_UNDO` là gì và tại sao nó lại quan trọng khi sử dụng System V Semaphores?
7.  Hàm `semctl()` được dùng để làm gì? Nêu hai lệnh phổ biến của nó.
8.  Trong ví dụ `sem_example.cpp`, tại sao tiến trình đầu tiên phải có một đối số và chịu trách nhiệm khởi tạo/xóa semaphore? Điều gì sẽ xảy ra nếu nó không làm vậy?

-----

### **Bài tập Thực hành Module 1 ✍️**

1.  **Chương trình "Giới hạn Tài nguyên Đa tiến trình":**

      * Viết một chương trình C++ (`resource_limiter.cpp`) mà:
          * Tạo một System V Semaphore nhị phân (khởi tạo giá trị 1).
          * Trong `main()`, `fork()` ra 3 tiến trình con.
          * Mỗi tiến trình con sẽ cố gắng vào một "vùng găng" 5 lần.
          * Trong vùng găng:
              * In ra "Process [PID] entered critical section."
              * Ngủ ngẫu nhiên 1-3 giây.
              * In ra "Process [PID] exited critical section."
          * Đảm bảo việc vào/ra vùng găng được bảo vệ bởi semaphore.
          * Tiến trình cha sẽ chờ tất cả các con hoàn thành và sau đó xóa semaphore.
      * **Thử thách:**
          * Chạy chương trình và quan sát output để thấy các tiến trình lần lượt vào vùng găng.
          * Thử thay đổi giá trị khởi tạo của semaphore thành 2 hoặc 3 (để cho phép nhiều tiến trình vào vùng găng đồng thời) và quan sát output.

2.  **Chương trình "Producer-Consumer Đơn giản với Semaphore":**

      * **Mục tiêu:** Sử dụng System V Semaphore để đồng bộ hóa một producer và một consumer.
      * **`producer_sem.cpp`:**
          * Tạo một System V Semaphore nhị phân (khởi tạo giá trị 0).
          * Trong vòng lặp, "sản xuất" một item (ví dụ: in "Producing item...").
          * Sau khi sản xuất, gọi `V()` trên semaphore để báo hiệu item có sẵn.
          * Ngủ một khoảng thời gian ngắn.
      * **`consumer_sem.cpp`:**
          * Lấy ID của semaphore.
          * Trong vòng lặp, gọi `P()` trên semaphore để chờ item có sẵn.
          * Sau khi `P()` thành công, "tiêu thụ" item (ví dụ: in "Consuming item...").
          * Ngủ một khoảng thời gian ngắn.
      * **Thử thách:**
          * Chạy `consumer_sem` trước, nó sẽ bị chặn.
          * Sau đó chạy `producer_sem`. Quan sát cách chúng đồng bộ hóa.
          * Đảm bảo semaphore được xóa sau khi hoàn tất.

3.  **Bài tập "Xử lý Semaphore bị kẹt" (Nâng cao):**

      * Chạy `sem_example.cpp 1` (tiến trình tạo semaphore) và sau đó `kill -9` nó khi nó đang ở trong vùng găng (giữa hai ký tự `X`).
      * **Mục tiêu:** Quan sát rằng semaphore không bị kẹt (do `SEM_UNDO`).
      * **Thử thách:** Tạm thời bỏ `SEM_UNDO` trong `sem_example.cpp` (chỉ để học, không làm trong thực tế\!), biên dịch lại và thử lại. Quan sát rằng semaphore bị kẹt và các tiến trình khác không thể truy cập. Sau đó, dùng lệnh `ipcs -s` và `ipcrm -s <semid>` để xóa semaphore bị kẹt.

-----



