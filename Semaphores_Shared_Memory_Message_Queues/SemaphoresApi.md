### ## 🏷️ Named Semaphores (Semaphore Có Tên)

Loại này dùng để đồng bộ hóa giữa các **tiến trình độc lập** (unrelated processes) vì chúng được nhận dạng bằng một cái tên chung trên toàn hệ thống.

**Vòng đời (Lifecycle):** `sem_open` ➡️ `sem_wait`/`sem_post` ➡️ `sem_close` (mỗi tiến trình) ➡️ `sem_unlink` (chỉ một tiến trình).

#### **API chính:**

1.  **`sem_t* sem_open(const char* name, int oflag, mode_t mode, unsigned int value);`**
    * **Mục đích:** Tạo mới một semaphore hoặc mở một semaphore đã tồn tại.
    * **`name`**: Tên của semaphore, là một chuỗi bắt đầu bằng `/`. Ví dụ: `/my_named_sem`.
    * **`oflag`**: Cờ tùy chọn. Quan trọng nhất là **`O_CREAT`** (tạo nếu chưa có) và **`O_EXCL`** (báo lỗi nếu đã tồn tại khi dùng chung với `O_CREAT`).
    * **`mode`**: Quyền truy cập (ví dụ `0666`), chỉ cần thiết khi dùng `O_CREAT`.
    * **`value`**: Giá trị khởi tạo cho semaphore, chỉ cần thiết khi dùng `O_CREAT`.
    * **Trả về:** Một con trỏ tới semaphore nếu thành công, hoặc `SEM_FAILED` nếu thất bại.

2.  **`int sem_close(sem_t* sem);`**
    * **Mục đích:** Đóng kết nối từ tiến trình hiện tại tới semaphore. Nó không xóa semaphore khỏi hệ thống.
    * **`sem`**: Con trỏ được trả về từ `sem_open`.
    * **Trả về:** `0` nếu thành công, `-1` nếu thất bại.

3.  **`int sem_unlink(const char* name);`**
    * **Mục đích:** **Xóa semaphore ra khỏi hệ thống**. Sau khi được gọi, các tiến trình khác sẽ không thể `sem_open` tới tên này nữa. Semaphore sẽ bị hủy hoàn toàn khi không còn tiến trình nào `sem_close` nó.
    * **`name`**: Tên của semaphore cần xóa.
    * **Trả về:** `0` nếu thành công, `-1` nếu thất bại.

---

### ## 🧠 Unnamed Semaphores (Semaphore Không Tên)

Loại này dùng để đồng bộ hóa giữa các **luồng** (threads) trong cùng một tiến trình, hoặc giữa các **tiến trình có liên quan** (ví dụ cha-con) đã chia sẻ vùng nhớ. Chúng chỉ là một object trong bộ nhớ.

**Vòng đời (Lifecycle):** `sem_init` ➡️ `sem_wait`/`sem_post` ➡️ `sem_destroy`.

#### **API chính:**

1.  **`int sem_init(sem_t* sem, int pshared, unsigned int value);`**
    * **Mục đích:** Khởi tạo một semaphore không tên.
    * **`sem`**: Con trỏ tới một biến kiểu `sem_t` mà bạn đã khai báo trong code. Semaphore sẽ được khởi tạo tại địa chỉ này.
    * **`pshared`**: Một cờ quan trọng:
        * Nếu **`pshared = 0`**: Semaphore chỉ được chia sẻ giữa các luồng của tiến trình hiện tại.
        * Nếu **`pshared != 0`**: Semaphore có thể được chia sẻ giữa các tiến trình, với điều kiện biến `sem_t` phải nằm trong một vùng nhớ chia sẻ (shared memory).
    * **`value`**: Giá trị khởi tạo cho semaphore.
    * **Trả về:** `0` nếu thành công, `-1` nếu thất bại.

2.  **`int sem_destroy(sem_t* sem);`**
    * **Mục đích:** Hủy một semaphore không tên đã được khởi tạo bằng `sem_init`, giải phóng tài nguyên mà nó chiếm giữ.
    * **Lưu ý:** Cố gắng hủy một semaphore mà đang có tiến trình/luồng bị block trên đó sẽ gây ra hành vi không xác định (undefined behavior).
    * **`sem`**: Con trỏ tới semaphore cần hủy.
    * **Trả về:** `0` nếu thành công, `-1` nếu thất bại.

---

### ## ⚖️ Bảng so sánh nhanh

| Đặc điểm | Named Semaphores | Unnamed Semaphores |
| :--- | :--- | :--- |
| **Trường hợp dùng** |  между независимыми процессами (Between independent processes) | между потоками или связанными процессами (Between threads or related processes) |
| **Định danh** | Chuỗi tên (`/my_sem`) | Con trỏ tới biến `sem_t` |
| **API Tạo** | `sem_open()` | `sem_init()` |
| **API Hủy** | `sem_close()` & `sem_unlink()` | `sem_destroy()` |
| **Sự tồn tại** | Tồn tại trên hệ thống cho đến khi `sem_unlink` được gọi | Chỉ tồn tại trong bộ nhớ, mất khi tiến trình kết thúc hoặc `sem_destroy` |
| **Hiệu năng** | Chậm hơn một chút (liên quan đến file system) | Nhanh hơn (chỉ là thao tác bộ nhớ) |