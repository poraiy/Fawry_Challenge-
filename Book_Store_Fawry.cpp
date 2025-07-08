
// we have three types of books each type have some data and if it is for sale and if we can buy it 
// 




#include <iostream>
#include <vector>
#include <memory>
#include <unordered_map>
#include <stdexcept>
#include <ctime>

using namespace std;
//1 
void print(const string& msg) {
    cout << "Quantum Bookstore: " << msg << endl;        
}




//2 
//الكتاب العادى و لا ورقى و لا الكترونى و لا للعرض    له اى دى  و  عنوان  و  كاتب و سنة نشر و سعر  و عملنا ال  جيترز الى محتاجنها و عملنا فنكشن  تعرفنا هل الكتاب قابل للبيع و لا لا و فاكشن تانية  عشان نعرف عملية الشراء جايزة ولا ا بناءا على ال الكمية المطلوبة 
class Book {
protected:
    string isbn, title, author;
    int year;
    double price;

public:
    Book(string isbn, string title, string author, int year, double price)
        : isbn(isbn), title(title), author(author), year(year), price(price) {}

    virtual ~Book() = default;

    string getISBN() const { return isbn; }
    int getYear() const { return year; }
    double getPrice() const { return price; }
    virtual bool isForSale() const = 0;
    virtual void purchase(int quantity, const string& email, const string& address) = 0;
};



//3
//الكتاب الورقى  :  و المعلومة الاضافية هيا الكمية المتاحة من الكتاب الورقى    و الى على اساسها بنحدد هل ينفع نشترية و لا لا لان المستقدم مش هيشترى كتاب المستخدم هيشترى كمية كتب 
// الكتابمن النوع ده قابل للبيع 
// عملية الشراء ناجهة لو الكمية الى عايز اشتريها كا مستخدم اقل من الكمية المتاحة 
// inherited of book class
class PaperBook : public Book {
    int stock;

public:
    PaperBook(string isbn, string title, string author, int year, double price, int stock)
        : Book(isbn, title, author, year, price), stock(stock) {}

    bool isForSale() const override { return true; }

    void purchase(int quantity, const string& email, const string& address) override {
        if (quantity > stock)
            throw runtime_error("Not enough stock!");
        stock -= quantity;
        print("Paper book purchased. Shipping to: " + address);  // للشحن 
        print("Total paid: " + to_string(price * quantity));// عرض السعر 
    }

    int getStock() const { return stock; }
};


//4 
//الكتاب الالكترونى  :  و المعلومة الاضافية الى فيه هيا ان عبارة عن فايل و الفايل ليه نوع 
// inherited of book class
// هوا قابل للبيع 
// و عملية الشراء هتم لو الكمية اقل = 1 
class EBook : public Book {
    string fileType;

public:
    EBook(string isbn, string title, string author, int year, double price, string fileType)
        : Book(isbn, title, author, year, price), fileType(fileType) {}

    bool isForSale() const override { return true; }

    void purchase(int quantity, const string& email, const string& address) override {
        if (quantity != 1)
            throw runtime_error("Can only buy one copy of an EBook");
        print("EBook purchased. Sending to email: " + email);
        print("Total paid: " + to_string(price));
    }
};



//5
// الكتاب للعرض فقط 
// غير قابل للبيع 
// اذا تمت محاولة شراؤه   يتم عرض رسالة خطأ
class ShowcaseBook : public Book {
public:
    ShowcaseBook(string isbn, string title, string author, int year)
        : Book(isbn, title, author, year, 0.0) {}

    bool isForSale() const override { return false; }

    void purchase(int quantity, const string& email, const string& address) override {
        throw runtime_error("This book is not for sale.");
    }
};



//6
// the store class which has inventory which contain all books added by addBook function 
// and  have removeOutdatedBooks function which responsible for deleting all outdated books from inventory 
class QuantumBookstore {
    unordered_map<string, shared_ptr<Book>> inventory;

public:
// المستخدم هيضيف مجموعة مت الكتب و بانواها المختلفة 
    void addBook(shared_ptr<Book> book) {
        inventory[book->getISBN()] = book;
    }
   // in inventory the admin may added out dated book so we will go throw the inventory and remove outdated books in put them in removed vector and delet from inventory 
       vector<shared_ptr<Book>> removeOutdatedBooks(int maxAge) {  // 
        vector<shared_ptr<Book>> removed;
        int currentYear = time(0) / 31556926 + 1970;

        for (auto it = inventory.begin(); it != inventory.end();) {  // loop throw inventory 
            if (currentYear - it->second->getYear() > maxAge) {  // if the book in inentory is outdated 
                removed.push_back(it->second);                // put in removed vector 
                it = inventory.erase(it);                     // delet book from inventory 
            } else {
                ++it;                                         // go to next element 
            }
        } 
        return removed;                                       // return removed vector  and in main we will loop throw the removed vector and show all removed 
    }

    void buyBook(string isbn, int quantity, string email, string address) {
        if (inventory.find(isbn) == inventory.end())    // if book is not found  
            throw runtime_error("Book not found!");

        shared_ptr<Book> book = inventory[isbn];       // 

        if (!book->isForSale())
            throw runtime_error("Book is not for sale!");

        book->purchase(quantity, email, address);
    }
};





//7
class QuantumBookstoreFullTest {
public:
    static void run() {
        QuantumBookstore store;

        store.addBook(make_shared<PaperBook>("001", "C++ Primer", "Lippman", 2010, 50.0, 10));
        store.addBook(make_shared<EBook>("002", "Learn Python", "Guido", 2015, 30.0, "PDF"));
        store.addBook(make_shared<ShowcaseBook>("003", "Ancient Scripts", "Unknown", 1800));

        try {
            store.buyBook("001", 2, "user@example.com", "123 Main St");
            store.buyBook("002", 1, "user@example.com", "");
            store.buyBook("003", 1, "user@example.com", "123 Main St"); // Will throw
        } catch (exception& e) {
            print(string("Error: ") + e.what());
        }

        auto removed = store.removeOutdatedBooks(50);
        for (auto& book : removed) {
            print("Removed outdated book: " + book->getISBN());
        }
    }
};

//8
int main() {
    QuantumBookstoreFullTest::run();
    return 0;
}
