
//  لو سمحت اقرأ الكومنتس 


#include <bits/stdc++.h>
#include <ctime>
#include <iomanip>
using namespace std;
//1 
class IShippable {   
public:
    virtual string getName() const = 0;   // virtual as we can make overrride it in inhereted class 
    virtual double getWeight() const = 0;
    virtual ~IShippable() {}
};


// كل منتج و ليه نوع اما منتج عادى  او منتج  ممكن تنتهى صلحيتة او منتج  بيتشحن او منتج  ممكن تنتهى صلحتة و بيشحن و تحت فى ال مان  لما بنعرف ال المنتج بناخد ابجكت من الكلاس المناس ب ليه او النوع المناسب 

//2 
// ده المنتج العادى ولا صلحيتة ينفع تنتهى و لا بيتشحن 
class Product {
protected:
    string name;
    double price;
    int quantity;

public:                                                 // pubilc ==> accesed any where 
    Product(string name, double price, int quantity)   // constructor to intialize name price  quantity 
        : name(name), price(price), quantity(quantity) {}

    virtual bool isExpired() const { return false; }    // function and we can overred it 
    virtual bool isShippable() const { return false; }  // function and we can overred it 

    // we have to make getters because of the encapsulation and the data is protected whec we can intialize throw constructors and can get throw these getters         
    string getName() const { return name; }      
    double getPrice() const { return price; }
    int getQuantity() const { return quantity; }

    void reduceQuantity(int q) { quantity -= q; } // this function is really important to be able to reduce quantity of product 
    virtual ~Product() {}                         // virtusl to be able to override this  destructor in other inherited class 
};


//3
// ده منتج ممكن صلحيته تتهى 
class ExpirableProduct : public Product {   // this is an inherited class of poduct which contain all public and protected data in product class  
    time_t expiryDate;                      // time_t is datatype to time from 1970 to expiryDate's  momment 

public:
    ExpirableProduct(string name, double price, int quantity, time_t expiryDate) // constructor  to intialized 
        : Product(name, price, quantity), expiryDate(expiryDate) {}   

    bool isExpired() const override {     
        return time(nullptr) > expiryDate;  //  time_t is datatype to time from 1970 to this   momment  
    }
};

// 4
// ده منتج بيتشحن 
class ShippableProduct : public Product, public IShippable {  
    double weight;         // بنحتاج نعرف الوزن لو المنتج بيتشحن 

public:
    ShippableProduct(string name, double price, int quantity, double weight)
        : Product(name, price, quantity), weight(weight) {}

    bool isShippable() const override { return true; }  // هنا انا بقول ان العنصر الى من الكلاس ده بيتشحن 
    string getName() const override { return name; }
    double getWeight() const override { return weight; }
};


//5 
// ده منتج قابل لانتهاء الصلاحية و قابل للشحن 
class ExpirableShippableProduct : public ExpirableProduct, public IShippable {  // inhereted from ExpirableProduct  but we will add weight just
    double weight;

public:
    ExpirableShippableProduct(string name, double price, int quantity, time_t expiryDate, double weight)  // constructor 
        : ExpirableProduct(name, price, quantity, expiryDate), weight(weight) {}    // weight 

    bool isShippable() const override { return true; }                             // بقول هنا انو   قابل للشحن و طب ليه مقولتش انو ممكن ينتهى الصلاحية  عشان كدا كدا مورث و الدالة بتاع الصلاحية موجودة فى الكلاس الاب 
    string getName() const override { return name; }
    double getWeight() const override { return weight; }
};


// 6
class Customer {   // العميل 
public:
    string name;
    double balance;  //رصيد العميل

    Customer(string name, double balance) : name(name), balance(balance) {}   
};


// 7
class Cart {  // دى بقا السلة  
                
    map<Product*, int> items; // بعمل ماب 

public:
    void add(Product* p, int qty) {
        if (qty <= 0 || qty > p->getQuantity()) {  // السلة و اللى هنضيف عليها منتجات لازم المية الى بضفا اكبر من 0  و الكمية الموجودة اكتر او بتساوى المطلوب
            throw runtime_error("Invalid quantity for product: " + p->getName());
        }
        items[p] += qty; // لو كلة تمام ببدأ اضيف فى السلة 
    }

    bool isEmpty() const { return items.empty(); }
    const map<Product*, int>& getItems() const { return items; }  //  برجع الماب الى فيها المنتجات و الكمية  بالفنكشن دى 
    void clear() { items.clear(); }  
};

// 8 
// خدمات الشحن  (عرض الوزن الكلى )
class ShippingService {
public:
    static void ship(const vector<IShippable*>& items) {   // after getting name and weight from Shippable class  we can now use it ass data type 
        cout << "** shipment notice **" << endl;
        double totalWeight = 0;
        for (auto item : items) {
            double w = item->getWeight();
            cout << "1x " << item->getName() << " " << w * 1000 << "g" << endl;
            totalWeight += w;      // بنحسب الوزن  الكلى للعناصر التى تشحن 
        }
        cout << fixed << setprecision(1);
        cout << "Total package weight " << totalWeight << "kg" << endl;
    }
};


// 9
//  الشراء 
class CheckoutSystem {
public:
    static void checkout(Customer& customer, Cart& cart) {     // العميل و السلة   
        if (cart.isEmpty()) throw runtime_error("Cart is empty");

        double subtotal = 0, shipping = 0;
        vector<IShippable*> toShip;

        for (const auto& [product, qty] : cart.getItems()) {  // هنلف على العناصر عنصر عنصر  
            if (product->isExpired())        // و نشوف لو منتهى الصلاحية 
                throw runtime_error("Product expired: " + product->getName());
            if (qty > product->getQuantity())    // لو الكمية مش كافية 
                throw runtime_error("Out of stock: " + product->getName());
             // غير كدا بنبدأ نجمع السعر  
            subtotal += qty * product->getPrice();

            if (product->isShippable()) {
                IShippable* shipItem = dynamic_cast<IShippable*>(product);  //   بنحط العناصر الى بتتشحن فى مكان واحد عشان نحدد السعر  بتاعها 
                for (int i = 0; i < qty; i++) {
                    toShip.push_back(shipItem);
                    shipping += shipItem->getWeight() * 10;   //  حددنا سكر الشحن 
                }
            }
        }

        double total = subtotal + shipping;    // السعر ى الكلى 
        if (customer.balance < total) throw runtime_error("Insufficient balance!");   //  بنتأكد من رصيد العميل 

        customer.balance -= total;   //  بنطرح الفلوس من رصيد العميل 
        for (const auto& [product, qty] : cart.getItems()) {
            product->reduceQuantity(qty);    //  بنطرح الكميات الى اتاخدت من المنتاجات 
        }

        if (!toShip.empty()) ShippingService::ship(toShip);

        cout << "** Checkout receipt **" << endl;       //  الفتورة اللذيذة  
        for (const auto& [product, qty] : cart.getItems()) {
            cout << qty << "x " << product->getName() << " " << qty * product->getPrice() << endl;  
        }
        cout << "----------------------" << endl; //  النهاية السعيدة 
        cout << "Subtotal " << subtotal << endl;
        cout << "Shipping " << shipping << endl;
        cout << "Amount " << total << endl;
        cout << "Balance " << customer.balance << endl;
        cout << "END." << endl;

        cart.clear();
    }
};


// 10 
int main() {
    try {
        time_t futureDate = time(nullptr) + 60 * 60 * 24 * 365;   

        Product* cheese = new ExpirableShippableProduct("Cheese", 100, 5, futureDate, 0.2);  //  االمنتج الاول 
        Product* biscuits = new ExpirableShippableProduct("Biscuits", 150, 2, futureDate, 0.7);  //  المنتج التانى 
        Product* tv = new ShippableProduct("TV", 1000, 3, 5.0);                                  // المنتج التالت 
        Product* scratchCard = new Product("Scratch Card", 50, 10);                                //   المنتج الرابع 

        Customer customer("Ali", 1000);   // hguldg 
        Cart cart;
    //    بنضيف المنتاجات فى العناصر 
        cart.add(cheese, 2);
        cart.add(biscuits, 1);
        cart.add(scratchCard, 1);
        // عشان نحسب الفتورة و نعرضها 
        CheckoutSystem::checkout(customer, cart);
    }
    catch (const exception& ex) {
        cerr << "Error: " << ex.what() << endl;
    }

    return 0;
}




