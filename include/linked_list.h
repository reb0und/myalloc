#ifndef LINKED_LIST_H
#define LINKED_LIST_H

template <typename T>
struct Node {
    T data;
    Node<T>* next;
    Node (T data) : data(data), next(0) {}; 
};

template <typename T>
class LinkedList {
    private:
        Node<T>* head;
    public:
        LinkedList() : head(0) {}

        ~LinkedList() {
            Node<T>* current = head;
            Node<T>* next = 0;

            while(current) {
                next = current->next;
                delete current;
                current = next;
            }
        }

        Node<T>* get_head() const {
            return head;
        }

        void set_head(Node<T>* newHead) {
            head = newHead;
        }

        void insertAtBeginning(const T data) {
            Node<T>* newNode = new Node<T>(data);
            newNode->next = head;
            head = newNode;
        }

        void insertAtEnd(const T data) {
            Node<T>* newNode = new Node<T>(data);
            if(!head) { 
                head = newNode;
            } else {
                Node<T>* temp = head;
                while (temp->next) {
                    temp = temp->next;
                }

                temp->next = newNode;
            }
        }

        void deleteNode(Node<T>* node) {
        if (!head || !node) {
            return;
        }

        if (head == node) {
            head = head->next;
            delete node;
            return;
        }

        Node<T>* current = head;
        while (current && current->next != node) {
            current = current->next;
        }

        if(!current) {
            return;
        }

        current->next = node->next;
        delete node;
    }
};

#endif