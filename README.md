# LAPORAN PENJELASAN SOAL SHIFT MODUL 3
## Soal 1
Buatlah program C yang bisa menghitung faktorial secara parallel lalu menampilkan hasilnya secara berurutan

Contoh:
```
    ./faktorial 5 3 4
    3! = 6
    4! = 24
    5! = 120
```
### Jawab :

  Sorting argumen ke index yang benar. (Menggunakan index sort)
```
      for (int i=1; i < argc ; i++){
          n = atoi(argv[i]);                                            // ubah argumen menjadi angka
          ...
          pthread_create(&tid[n], NULL, &factorial,(void *) &args[i]);  // nilai argumen menjadi index thread
```

  Panggil thread untuk menghitung faktorial berdasarkan index yang benar.
```
    void* factorial( void* args)
    ....
    unsigned long long  res = 1;
    for( i = n; i > 1; i--) res = res * i;
```
  Thread akan di-halt dengan mutex yang di inisialisasi sebagai -1. Jika nilai mutex sama dengan nilai index thread, maka thread akan   dijalankan kembali
```
    int mutex = -1;
    ....
    void* factorial( void* args)
    ....
    while(mutex != n){}
```

  Naikkan nilai mutex dan lakukan join thread sesuai urutan.

	for ( mutex = 0; mutex < 100; mutex++){ pthread_join (tid[mutex], NULL);}

## Soal 2 
Pada suatu hari ada orang yang ingin berjualan 1 jenis barang secara private, dia memintamu membuat program C dengan spesifikasi sebagai berikut:

- Terdapat 2 server: server penjual dan server pembeli
- 1 server hanya bisa terkoneksi dengan 1 client
- Server penjual dan server pembeli memiliki stok barang yang selalu sama
- Client yang terkoneksi ke server penjual hanya bisa menambah stok
  - Cara menambah stok: client yang terkoneksi ke server penjual mengirim string “tambah” ke server lalu stok bertambah 1
- Client yang terkoneksi ke server pembeli hanya bisa mengurangi stok
  - Cara mengurangi stok: client yang terkoneksi ke server pembeli mengirim string “beli” ke server lalu stok berkurang 1
- Server pembeli akan mengirimkan info ke client yang terhubung dengannya apakah transaksi berhasil atau tidak berdasarkan ketersediaan stok
  - Jika stok habis maka client yang terkoneksi ke server pembeli akan mencetak “transaksi gagal”
  - Jika stok masih ada maka client yang terkoneksi ke server pembeli akan mencetak “transaksi berhasil”
- Server penjual akan mencetak stok saat ini setiap 5 detik sekali
- __Menggunakan thread, socket, shared memory__


### Jawab :

#### Server 
  Buatlah socket sesuai spesifikasi yang diperlukan, jangan lupa untuk membedakan port server.
    
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);                                        // Buat socket ke internet dengan koneksi tcp
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));   
    ibind(server_fd, (struct sockaddr *)&address, (socklen_t) addrlen);                 // bind socket sesuai address & port yg diinginkan
    listen(server_fd, 1);                                                               // terima 1 koneksi saja
    
  Buatlah nilai stok yang dijadikan shared memory untuk mensinkronisasi nilai stok di server pembeli dan penjual
  
      int *stock;
      key_t key = 1234;
      int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
      stock = shmat(shmid, NULL, 0);
    
  Buat thread untuk meng-handle koneksi dari client.
  
    while( (new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) ){
        pthread_create(&tid[0], NULL, &conn, (void*) &new_socket);
        ...
    void* conn(void* args) {
          int valread, new_socket = *(int*) args;
          char buffer[1024] = {0};                                                   // buffer untuk menyimpan pesan dari client
          valread = read( new_socket , buffer, 1024);                                // baca pesan dari client
          ...
          int temp = *stock;
          char *hello = temp > 0 ? "transaksi berhasil" : "transaksi gagal";         // buffer untuk menyimpan pesan ke client
          send(new_socket , hello , strlen(hello) , 0 );                             // kirim pesan ke client
          
   
   #### Server Pembeli
   
   Di server pembeli, jika server menerima pesan "beli" stock akan berkurang jika ada dan akan mengirimkan pesan bahwa transaksi berhasil. Jika stock habis server akan mengirimkan pesan bahwa transaksi gagal
   
        ....
        else if (strcmp(buffer,"beli") == 0) {
            int temp = *stock;
            char *hello = temp > 0 ? "transaksi berhasil" : "transaksi gagal";    // Jika stock < 0, transaksi gagal
            if (strstr(hello, "berhasil")) {                                      // Jika stock > 0, transaksi berhasil dan stock berkurang
                *stock= *stock - 1;
            }
            send(new_socket , hello , strlen(hello) , 0 );
   
  #### Server Penjual
    
   Di server penjual, jika server menerima pesan  "tambah" stock akan bertambah. Selain itu, pada server penjual perlu dibuatkan thread untuk menampilkan stok setiap 5 detik.
  
        void* printStock( void* args){
          int i;
          while(1){
              if (i++%3 == 0) {
                  printf("Stok barang: %d\n", *stock);
              }
              sleep(1);
          }
        }
  
### Client
 Buatlah socket sesuai spesifikasi yang diperlukan, jangan lupa untuk menginputkan port ke server yang sesuai
 
    sock = socket(AF_INET, SOCK_STREAM, 0);                           // buat socket
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);             // tentukan alamat yang diinginkan
    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));  // buat koneksi ke server

  Terima pesan dan kirim pesan ke server.
    
     send(sock , hello , strlen(hello) , 0 );      // kirim pesan ke server, jika client pembeli, maka "beli" akan dikirimkan
                                                   // jika client penjual, maka "tambah" akan dikirimkan
     valread = read( sock , buffer, 1024);         // baca pesan dari server untuk mengetahui keberhasilan transaksi


## Soal 3
Agmal dan Iraj merupakan 2 sahabat yang sedang kuliah dan hidup satu kostan, sayangnya mereka mempunyai gaya hidup yang berkebalikan, dimana Iraj merupakan laki-laki yang sangat sehat,rajin berolahraga dan bangun tidak pernah kesiangan sedangkan Agmal hampir menghabiskan setengah umur hidupnya hanya untuk tidur dan ‘ngoding’. Dikarenakan mereka sahabat yang baik, Agmal dan iraj sama-sama ingin membuat satu sama lain mengikuti gaya hidup mereka dengan cara membuat Iraj sering tidur seperti Agmal, atau membuat Agmal selalu bangun pagi seperti Iraj. Buatlah suatu program C untuk menggambarkan kehidupan mereka dengan spesifikasi sebagai berikut:

- Terdapat 2 karakter Agmal dan Iraj
- Kedua karakter memiliki status yang unik
- Agmal mempunyai WakeUp_Status, di awal program memiliki status 0
- Iraj memiliki Spirit_Status, di awal program memiliki status 100
- Terdapat 3 Fitur utama
    - All Status, yaitu menampilkan status kedua sahabat

        Ex:
        
            Agmal WakeUp_Status = 75 
            Iraj Spirit_Status = 30
    - “Agmal Ayo Bangun” menambah WakeUp_Status Agmal sebesar 15 point
    - “Iraj Ayo Tidur” mengurangi Spirit_Status Iraj sebanyak 20 point
- Terdapat Kasus yang unik dimana:
    - Jika Fitur “Agmal Ayo Bangun” dijalankan sebanyak 3 kali, maka Fitur “Iraj Ayo Tidur” Tidak bisa dijalankan selama 10 detik (Dengan mengirim pesan ke sistem “Fitur Iraj Ayo Tidur disabled 10 s”)
    - Jika Fitur  “Iraj Ayo Tidur” dijalankan sebanyak 3 kali, maka Fitur “Agmal Ayo Bangun” Tidak bisa dijalankan selama 10 detik (Dengan mengirim pesan ke sistem “Agmal Ayo Bangun disabled 10 s”)
- Program akan berhenti jika Salah Satu :
    - WakeUp_Status Agmal >= 100 (Tampilkan Pesan “Agmal Terbangun,mereka bangun pagi dan berolahraga”)
    - Spirit_Status Iraj <= 0 (Tampilkan Pesan “Iraj ikut tidur, dan bangun kesiangan bersama Agmal”)
- __Syarat Menggunakan Lebih dari 1 Thread__


### Jawab :



## soal 4 
Buatlah sebuah program C dimana dapat menyimpan list proses yang sedang berjalan (ps -aux) maksimal 10 list proses. Dimana awalnya list proses disimpan dalam di 2 file ekstensi .txt yaitu  SimpanProses1.txt di direktori /home/Document/FolderProses1 dan SimpanProses2.txt di direktori /home/Document/FolderProses2 , setelah itu masing2 file di  kompres zip dengan format nama file KompresProses1.zip dan KompresProses2.zip dan file SimpanProses1.txt dan SimpanProses2.txt akan otomatis terhapus, setelah itu program akan menunggu selama 15 detik lalu program akan mengekstrak kembali file KompresProses1.zip dan KompresProses2.zip 

__Dengan Syarat__ :
- Setiap list proses yang di simpan dalam masing-masing file .txt harus berjalan bersama-sama
- Ketika mengkompres masing-masing file .txt harus berjalan bersama-sama
- Ketika Mengekstrak file .zip juga harus secara bersama-sama
- Ketika Telah Selesai melakukan kompress file .zip masing-masing file, maka program akan memberi pesan “Menunggu 15 detik untuk mengekstrak kembali”
- Wajib Menggunakan Multithreading
- Boleh menggunakan system



### Jawab :
Agar thread dijalankan secara bersamaan, digunkan mutex untuk menunggu kedua thread selesai untuk melakukan langkah selanjutnya.

        int mutex = 0
        ....
        ++mutex;
        while(1){
            if (mutex == 2) 
                return 0;
         }

1. Buat direktori yang diperlukan.

        char dir[26] = "~/Documents/FolderProses";
        sprintf(dir,"%s%d",dir, n);
        char step1[70] = "mkdir";
        sprintf(step1,"%s %s",step1, dir);
        system(step1);
        
2. Lakukan ps -aux dan buat .txt yang diperlukan di folder yang dibuat.

       char filen[30] = "SimpanProses";
       sprintf(filen,"%s%d.txt", filen, n);
       char step2[70] = "ps -aux | head -n 11 >";
       sprintf(step2,"%s %s/%s", step2, dir, filen);
       system(step2);

3. Zip file .txt yang dibuat.

        char zip[30] = "KompresProses";
        sprintf(zip,"%s%d.zip", zip, n);
        char step3[100] = "zip";
        sprintf(step3,"%s -j %s/%s %s/%s", step3, dir, zip, dir, filen);
        system(step3);

4. Hapus file .txt yang asli.

        char step4[70] = "rm";
        sprintf(step4,"%s %s/%s", step4, dir, filen);
        system(step4);
        
 5. Unzip kembali .txt yang diarsip setelah 15 detik.
 
        printf("Menunggu 15 detik untuk mengekstrak kembali \n");
        sleep(15);
        char step5[120] = "unzip";
        sprintf(step5,"%s %s/%s -d %s", step5, dir, zip, dir);
        system(step5);
 
## soal 5
Angga, adik Jiwang akan berulang tahun yang ke sembilan pada tanggal 6 April besok. Karena lupa menabung, Jiwang tidak mempunyai uang sepeserpun untuk membelikan Angga kado. Kamu sebagai sahabat Jiwang ingin membantu Jiwang membahagiakan adiknya sehingga kamu menawarkan bantuan membuatkan permainan komputer sederhana menggunakan program C. Jiwang sangat menyukai idemu tersebut. Berikut permainan yang Jiwang minta. 
- Pemain memelihara seekor monster lucu dalam permainan. Pemain dapat  memberi nama pada monsternya.
- Monster pemain memiliki hunger status yang berawal dengan nilai 200 (maksimalnya) dan nanti akan berkurang 5 tiap 10 detik.Ketika hunger status mencapai angka nol, pemain akan kalah. Hunger status dapat bertambah 15 apabila pemain memberi makan kepada monster, tetapi banyak makanan terbatas dan harus beli di Market.
- Monster pemain memiliki hygiene status yang berawal dari 100 dan nanti berkurang 10 tiap 30 detik. Ketika hygiene status mencapai angka nol, pemain akan kalah. Hygiene status' dapat bertambah 30 hanya dengan memandikan monster. Pemain dapat memandikannya setiap 20 detik(cooldownnya 20 detik).
- Monster pemain memiliki health status yang berawal dengan nilai 300. Variabel ini bertambah (regenerasi)daa 5 setiap 10 detik ketika monster dalam keadaan standby.
- Monster pemain dapat memasuki keadaan battle. Dalam keadaan ini, food status(fitur b), hygiene status'(fitur c), dan ‘regenerasi’(fitur d) tidak akan berjalan. Health status dari monster dimulai dari darah saat monster pemain memasuki battle. Monster pemain akan bertarung dengan monster NPC yang memiliki darah 100. Baik monster pemain maupun NPC memiliki serangan sebesar 20. Monster pemain dengan monster musuh akan menyerang secara bergantian. 
- Fitur shop, pemain dapat membeli makanan sepuas-puasnya selama stok di toko masih tersedia.
    - Pembeli (terintegrasi dengan game)
        - Dapat mengecek stok makanan yang ada di toko.
        - Jika stok ada, pembeli dapat membeli makanan.
    - Penjual (terpisah)
        - Bisa mengecek stok makanan yang ada di toko
        - Penjual dapat menambah stok makanan.

Spesifikasi program:
- Program mampu mendeteksi input berupa key press. (Program bisa berjalan tanpa perlu menekan tombol enter)
- Program terdiri dari 3 scene yaitu standby, battle, dan shop.
- Pada saat berada di standby scene, program selalu menampilkan health status, hunger status, hygiene status, stok makanan tersisa, dan juga status kamar mandi (“Bath is ready” jika bisa digunakan, “Bath will be ready in [bath cooldown]s” jika sedang cooldown). Selain itu program selalu menampilkan 5 menu, yaitu memberi makan, mandi, battle, shop, dan exit.

    Contoh :

        Standby Mode
        Health : [health status]
        Hunger : [hunger status]
        Hygiene : [hygiene status]
        Food left : [your food stock]
        Bath will be ready in [cooldown]s
        Choices
        1. Eat
        2. Bath
        3. Battle
        4. Shop
        5. Exit

- Pada saat berada di battle scene, program selalu menampilkan health status milik pemain dan monster NPC. Selain itu, program selalu menampilkan 2 menu yaitu serang atau lari. 

    Contoh :

        Battle Mode
        Monster’s Health : [health status]
        Enemy’s Health : [enemy health status]
        Choices
        1. Attack
        2. Run

- Pada saat berada di shop scene versi pembeli, program selalu menampilkan food stock toko dan milik pemain. Selain itu, program selalu menampilkan 2 menu yaitu beli dan kembali ke standby scene. 

    Contoh :

        Shop Mode
        Shop food stock : [shop food stock]
        Your food stock : [your food stock]
        Choices
        1. Buy
        2. Back

- Pada program penjual, program selalu menampilkan food stock toko. Selain itu, program juga menampilkan 2 menu yaitu restock dan exit.

    Contoh :

        Shop
        Food stock : [shop food stock]
        Choices
        1. Restock
        2. Exit

- Pastikan terminal hanya mendisplay status detik ini sesuai scene terkait (hint: menggunakan system(“clear”))


### Jawab :
