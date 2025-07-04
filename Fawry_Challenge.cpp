#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <ctime>
#include <iomanip>

using namespace std;

class IShippable {
public:
    virtual string getName() const = 0;
    virtual double getWeight() const = 0;
    virtual ~IShippable() {}
};

class Product {
protected:
    string name;
    double price;
    int quantity;

public:
    Product(string name, double price, int quantity)
        : name(name), price(price), quantity(quantity) {}

    virtual bool isExpired() const { return false; }
    virtual bool isShippable() const { return false; }

    string getName() const { return name; }
    double getPrice() const { return price; }
    int getQuantity() const { return quantity; }

    void reduceQuantity(int q) { quantity -= q; }

    virtual ~Product() {}
};

class ExpirableProduct : public Product {
    time_t expiryDate;

public:
    ExpirableProduct(string name, double price, int quantity, time_t expiryDate)
        : Product(name, price, quantity), expiryDate(expiryDate) {}

    bool isExpired() const override {
        return time(nullptr) > expiryDate;
    }
};

class ShippableProduct : public Product, public IShippable {
    double weight;

public:
    ShippableProduct(string name, double price, int quantity, double weight)
        : Product(name, price, quantity), weight(weight) {}

    bool isShippable() const override { return true; }
    string getName() const override { return name; }
    double getWeight() const override { return weight; }
};

class ExpirableShippableProduct : public ExpirableProduct, public IShippable {
    double weight;

public:
    ExpirableShippableProduct(string name, double price, int quantity, time_t expiryDate, double weight)
        : ExpirableProduct(name, price, quantity, expiryDate), weight(weight) {}

    bool isShippable() const override { return true; }
    string getName() const override { return name; }
    double getWeight() const override { return weight; }
};

class Customer {
public:
    string name;
    double balance;

    Customer(string name, double balance) : name(name), balance(balance) {}
};

class Cart {
    map<Product*, int> items;

public:
    void add(Product* p, int qty) {
        if (qty <= 0 || qty > p->getQuantity()) {
            throw runtime_error("Invalid quantity for product: " + p->getName());
        }
        if (p->isExpired()) {
            throw runtime_error("Product expired: " + p->getName());
        }
        items[p] += qty;
    }

    bool isEmpty() const { return items.empty(); }

    const map<Product*, int>& getItems() const { return items; }

    void clear() { items.clear(); }
};

class ShippingService {
public:
    static void ship(const vector<IShippable*>& items) {
        cout << " Shipment notice " << endl;
        double totalWeight = 0;
        for (auto item : items) {
            double w = item->getWeight();
            cout << "1x " << item->getName() << " " << w * 1000 << "g" << endl;
            totalWeight += w;
        }
        cout << fixed << setprecision(1);
        cout << "Total package weight " << totalWeight << "kg" << endl;
    }
};

class CheckoutSystem {
public:
    static void checkout(Customer& customer, Cart& cart) {
        if (cart.isEmpty())
            throw runtime_error("Cart is empty!");

        double subtotal = 0;
        double shipping = 0;
        vector<IShippable*> toShip;

        for (const auto& [product, qty] : cart.getItems()) {
            if (product->isExpired())
                throw runtime_error("Product expired: " + product->getName());
            if (qty > product->getQuantity())
                throw runtime_error("Insufficient stock: " + product->getName());

            subtotal += qty * product->getPrice();

            if (product->isShippable()) {
                IShippable* shipItem = dynamic_cast<IShippable*>(product);
                for (int i = 0; i < qty; ++i) {
                    toShip.push_back(shipItem);
                    shipping += shipItem->getWeight() * 10;
                }
            }
        }

        double total = subtotal + shipping;
        if (customer.balance < total)
            throw runtime_error("Insufficient balance!");

        customer.balance -= total;
        for (const auto& [product, qty] : cart.getItems()) {
            product->reduceQuantity(qty);
        }

        if (!toShip.empty())
            ShippingService::ship(toShip);

        cout << " Checkout receipt " << endl;
        for (const auto& [product, qty] : cart.getItems()) {
            cout << qty << "x " << product->getName() << " " << qty * product->getPrice() << endl;
        }
        cout << "----------------------" << endl;
        cout << "Subtotal " << subtotal << endl;
        cout << "Shipping " << shipping << endl;
        cout << "Amount " << total << endl;
        cout << "Balance " << customer.balance << endl;
        cout << "END." << endl;

        cart.clear();
    }
};

int main() {
    try {
        
        time_t futureDate = time(nullptr) + 60 * 60 * 24 * 365; 

        Product* cheese = new ExpirableShippableProduct("Cheese", 100, 5, futureDate, 0.2);
        Product* biscuits = new ExpirableShippableProduct("Biscuits", 150, 2, futureDate, 0.7);
        Product* tv = new ShippableProduct("TV", 1000, 3, 5.0);
        Product* scratchCard = new Product("Scratch Card", 50, 10); 
        Customer customer("Ali", 1000);
        Cart cart;

        cart.add(cheese, 2);
        cart.add(biscuits, 1);
        cart.add(scratchCard, 1);

        CheckoutSystem::checkout(customer, cart);
    }
    catch (const exception& ex) {
        cerr << "Error: " << ex.what() << endl;
    }

    return 0;
}
