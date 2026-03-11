// Lab 5: Binary Trees & Binary Search Trees

// https://github.com/doctest/doctest/blob/master/doc/markdown/tutorial.md

#include <iostream>
#include <memory>
#include <cassert>
#include <algorithm>
#include <functional>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <queue>

#include "doctest.h"

// ************************ BinaryNode ************************

// BinaryNode for both BST and BNT.
template<class ItemType>
class BinaryNode final
{
private:
    ItemType                              item;          // Data portion

    std::shared_ptr<BinaryNode<ItemType>> leftChildPtr;  // Pointer to left child
    std::shared_ptr<BinaryNode<ItemType>> rightChildPtr; // Pointer to right child

public:
    BinaryNode() : item{}, leftChildPtr(nullptr), rightChildPtr(nullptr) {}
    explicit BinaryNode(const ItemType& anItem) : item(anItem), leftChildPtr(nullptr), rightChildPtr(nullptr) {}

    BinaryNode(const ItemType& anItem,
               std::shared_ptr<BinaryNode<ItemType>> leftPtr,
               std::shared_ptr<BinaryNode<ItemType>> rightPtr) : item(anItem), leftChildPtr(leftPtr), rightChildPtr(rightPtr) {
    }

    void setItem(const ItemType& anItem) {
        item = anItem;
    }
    ItemType getItem() const {
        return item;
    }

    bool isLeaf() const {
        // TODO: A leaf node has no children (both left and right pointers are null)
        return leftChildPtr == nullptr && rightChildPtr == nullptr;
    }

    auto getLeftChildPtr() const {
        return leftChildPtr;
    }
    auto getRightChildPtr() const {
        return rightChildPtr;
    }

    void setLeftChildPtr(std::shared_ptr<BinaryNode<ItemType>> leftPtr) {
        leftChildPtr = leftPtr;
    }
    void setRightChildPtr(std::shared_ptr<BinaryNode<ItemType>> rightPtr) {
        rightChildPtr = rightPtr;
    }
}; // end BinaryNode

// *************************** Helper Free Functions ******************************

// Helper methods for both tree implementations. These could be part of an internal
// helper library or rolled into the BinaryNode class as static methods.
template<class ItemType>
int heightAtTree(std::shared_ptr<BinaryNode<ItemType>> subTreePtr)
{
    // TODO: Height is the number of edges on the longest path from root to leaf
    // Base case: empty tree has height -1, single node has height 0
    // Recursive case: height = 1 + max(left_height, right_height)
    if (subTreePtr == nullptr)
        return -1;
    return 1 + std::max(
        heightAtTree(subTreePtr->getLeftChildPtr()),
        heightAtTree(subTreePtr->getRightChildPtr()));
}  // end getHeightHelper

template<class ItemType>
int nodeCountAtTree(std::shared_ptr<BinaryNode<ItemType>> subTreePtr) {
    if (subTreePtr == nullptr)
        return 0;

    return 1 + nodeCountAtTree(subTreePtr->getLeftChildPtr()) + nodeCountAtTree(subTreePtr->getRightChildPtr());
}

// Recursively searches for target value. Searches depth first, left to right.
// Returns the node containing the first occurrence of target value.
template<class ItemType>
std::shared_ptr<BinaryNode<ItemType>> findNodeInTree(std::shared_ptr<BinaryNode<ItemType>> treePtr, const ItemType& target) {
    if (treePtr == nullptr) {
        return nullptr;
    }

    if (treePtr->getItem() == target) {
        return treePtr;
    }

    // TODO: Search both left and right subtrees recursively
    // If found in left subtree, return that result
    // Otherwise, return result from right subtree search
    auto leftResult = findNodeInTree(treePtr->getLeftChildPtr(), target);
    if (leftResult)
        return leftResult;
    return findNodeInTree(treePtr->getRightChildPtr(), target);
}

template<class ItemType>
void preorder(std::function<void(ItemType&)> visit, std::shared_ptr<BinaryNode<ItemType>> treePtr) {
    if (treePtr != nullptr)
    {
        ItemType theItem = treePtr->getItem();
        visit(theItem);
        preorder(visit, treePtr->getLeftChildPtr());
        preorder(visit, treePtr->getRightChildPtr());
    }
}

template<class ItemType>
void inorder(std::function<void(ItemType&)> visit,
             std::shared_ptr<BinaryNode<ItemType>> treePtr)
{
    if (treePtr != nullptr)
    {
        inorder(visit, treePtr->getLeftChildPtr());
        ItemType theItem = treePtr->getItem();
        visit(theItem);
        inorder(visit, treePtr->getRightChildPtr());
    }
}

template<class ItemType>
void postorder(std::function<void(ItemType&)> visit, std::shared_ptr<BinaryNode<ItemType>> treePtr) {
    if (treePtr != nullptr)
    {
        postorder(visit, treePtr->getLeftChildPtr());
        postorder(visit, treePtr->getRightChildPtr());
        ItemType theItem = treePtr->getItem();
        visit(theItem);
    }
}

// You will likely need this for BST's remove.
// REF: https://www.geeksforgeeks.org/deletion-in-binary-search-tree/
template<class ItemType>
std::shared_ptr<BinaryNode<ItemType>> findInOrderSuccessor(std::shared_ptr<BinaryNode<ItemType>> node) {
    node = node->getRightChildPtr();
    while (node != nullptr && node->getLeftChildPtr() != nullptr) {
        node = node->getLeftChildPtr();
    }
    return node;
}

// Copies the tree rooted at treePtr and returns a pointer to the root of the copy.
template<class ItemType>
std::shared_ptr<BinaryNode<ItemType>> copyTree(const std::shared_ptr<BinaryNode<ItemType>> oldTreeRootPtr)
{
    std::shared_ptr<BinaryNode<ItemType>> newTreePtr;

    // Copy tree nodes during a preorder traversal. (Do a manual traversal using smart pointers, not the public traversal methods.)

    // TODO:
    // Base case: if oldTreeRootPtr is null, return null
    // Recursive case:
    //   1. Create new node with same item as oldTreeRootPtr
    //   2. Recursively copy left subtree and attach to new node
    //   3. Recursively copy right subtree and attach to new node
    //   4. Return the new node

    if (oldTreeRootPtr == nullptr) return nullptr;

    auto newNode = std::make_shared<BinaryNode<ItemType>>(oldTreeRootPtr->getItem());
    newNode->setLeftChildPtr(copyTree(oldTreeRootPtr->getLeftChildPtr()));
    newNode->setRightChildPtr(copyTree(oldTreeRootPtr->getRightChildPtr()));
    return newNode;
}  // end copyTree

// *************************** BinaryNodeTree ******************************

template<class ItemType>
class BinaryTreeInterface
{
public:
    /** Tests whether this binary tree is empty.
     @return  True if the binary tree is empty, or false if not. */
    virtual bool isEmpty() const = 0;

    /** Gets the height of this binary tree.
     @return  The height of the binary tree. */
    virtual int getHeight() const = 0;

    /** Gets the number of nodes in this binary tree.
     @return  The number of nodes in the binary tree. */
    virtual int getNumberOfNodes() const = 0;

    /** Gets the data that is in the root of this binary tree.
     @pre  The binary tree is not empty.
     @post  The root's data has been returned, and the binary tree is unchanged.
     @return  The data in the root of the binary tree. */
    virtual ItemType getRootData() const = 0;

    /** Replaces the data in the root of this binary tree with the given data,
     if the tree is not empty. However, if the tree is empty, inserts a new
     root node containing the given data into the tree.
     @pre  None.
     @post  The data in the root of the binary tree is as given.
     @param newData  The data for the root. */
    virtual void setRootData(const ItemType& newData) = 0;

    /** Adds the given data to this binary tree.
     @param newData  The data to add to the binary tree.
     @post  The binary tree contains the new data.
     @return  True if the addition is successful, or false if not. */
    virtual bool add(const ItemType& newData) = 0;

    /** Removes the specified data from this binary tree.
     @param target  The data to remove from the binary tree.
     @return  True if the removal is successful, or false if not. */
    virtual bool remove(const ItemType& target) = 0;

    /** Removes all data from this binary tree. */
    virtual void clear() = 0;

    /** Tests whether the specified data occurs in this binary tree.
     @post  The binary tree is unchanged.
     @param target  The data to find.
     @return  True if data matching the target occurs in the tree, or false if not. */
    virtual bool contains(const ItemType& target) const = 0;

    /** Traverses this binary tree in preorder (inorder, postorder) and
        calls the function visit once for each node.
     @param visit  A client-defined function that performs an operation on
        either each visited node or its data. */
    virtual void preorderTraverse(std::function<void(ItemType&)> visit) const = 0;
    virtual void inorderTraverse(std::function<void(ItemType&)> visit) const = 0;
    virtual void postorderTraverse(std::function<void(ItemType&)> visit) const = 0;


    /** Destroys this tree and frees its assigned memory. */
    virtual ~BinaryTreeInterface() = default;
}; // end BinaryTreeInterface

template<class ItemType>
class BinaryNodeTree final : public BinaryTreeInterface<ItemType>
{
private:
    std::shared_ptr<BinaryNode<ItemType>> rootPtr;

    // Recursively adds a new node to the tree in a left/right fashion to keep tree balanced.
    // Hint: Compare heights of left and right subtrees to decide where to add.
    std::shared_ptr<BinaryNode<ItemType>> balancedAdd(std::shared_ptr<BinaryNode<ItemType>> subTreePtr,
                     std::shared_ptr<BinaryNode<ItemType>> newNodePtr)
    {
        if (subTreePtr == nullptr)
            return newNodePtr;
        // TODO: To keep tree balanced, add to the subtree with smaller height
        // 1. Get height of left and right subtrees using heightAtTree()
        // 2. If left height <= right height, recursively add to left subtree
        // 3. Otherwise, recursively add to right subtree
        // 4. Update the appropriate child pointer and return current node
        int leftHeight = heightAtTree(subTreePtr->getLeftChildPtr());
        int rightHeight = heightAtTree(subTreePtr->getRightChildPtr());

        if (leftHeight <= rightHeight) {
            subTreePtr->setLeftChildPtr(balancedAdd(subTreePtr->getLeftChildPtr(), newNodePtr));
        } else {
            subTreePtr->setRightChildPtr(balancedAdd(subTreePtr->getRightChildPtr(), newNodePtr));
        }

        return subTreePtr;
    }  // end balancedAdd

public:
    //------------------------------------------------------------
    // Constructor and Destructor Section.
    //------------------------------------------------------------
    BinaryNodeTree() : rootPtr(nullptr)
    {
    }  // end default constructor

    explicit BinaryNodeTree(const ItemType& rootItem) : rootPtr(std::make_shared<BinaryNode<ItemType>>(rootItem, nullptr, nullptr))
    {
    } // end constructor

    BinaryNodeTree(const ItemType& rootItem,
                   const std::shared_ptr<BinaryNodeTree<ItemType>> leftTreePtr,
                   const std::shared_ptr<BinaryNodeTree<ItemType>> rightTreePtr)
            : rootPtr(std::make_shared<BinaryNode<ItemType>>(rootItem,
                                                             copyTree(leftTreePtr->rootPtr),
                                                             copyTree(rightTreePtr->rootPtr)))
    {
    } // end constructor

    BinaryNodeTree(const BinaryNodeTree<ItemType>& treePtr)
    {
        rootPtr = copyTree(treePtr.rootPtr);
    }  // end copy constructor

    ~BinaryNodeTree() override = default;

    //------------------------------------------------------------
    // Public BinaryTreeInterface Methods Section.
    //------------------------------------------------------------
    bool isEmpty() const override {
        return rootPtr == nullptr;
    }
    int getHeight() const override {
        return heightAtTree(rootPtr);
    }
    int getNumberOfNodes() const override {
        return nodeCountAtTree(rootPtr);
    }

    ItemType getRootData() const override {
        if (isEmpty()) {
            throw std::exception();
        }
        return rootPtr->getItem();
    }

    void setRootData(const ItemType& newData) override {
        if (isEmpty()) {
            rootPtr = std::make_shared<BinaryNode<ItemType>>(newData);
        }
        else {
            rootPtr->setItem(newData);
        }
    }

    bool add(const ItemType& newData) override {
        auto newNodePtr = std::make_shared<BinaryNode<ItemType>>(newData);
        rootPtr = balancedAdd(rootPtr, newNodePtr);

        return true;
    }  // end add

    // REF: https://www.geeksforgeeks.org/deletion-binary-tree/
    //
    // One approach is to swap the values of the node to a leaf, then delete the leaf. Which child you decide to
    // swap to would affect the shape of the resulting tree. Best to remove from the subtree with the greater height
    // for a more balanced result.
    //
    // Removes the target value from the BinaryNodeTree. Note this is for the regular, non-binary search tree.
    bool remove(const ItemType& target) override {
        bool removed = false;
        // TODO: This is for unordered binary tree removal (not BST!)
        // Suggested approach from GeeksForGeeks reference:
        // 1. Find the node to remove
        // 2. Find the deepest rightmost node (last node in level order)
        // 3. Replace target node's data with deepest node's data
        // 4. Delete the deepest node
        // Alternative: Create a helper function that returns new root after removal
        if (rootPtr == nullptr)
            return removed;

        std::queue<std::shared_ptr<BinaryNode<ItemType>>> nodeQueue;
        nodeQueue.push(rootPtr);

        std::shared_ptr<BinaryNode<ItemType>> targetNode = nullptr;
        std::shared_ptr<BinaryNode<ItemType>> lastNode = nullptr;

        while (!nodeQueue.empty()) {
            lastNode = nodeQueue.front();
            nodeQueue.pop();

            if (lastNode->getItem() == target) {
                targetNode = lastNode;
            }

            if (lastNode->getLeftChildPtr())
                nodeQueue.push(lastNode->getLeftChildPtr());
            if (lastNode->getRightChildPtr())
                nodeQueue.push(lastNode->getRightChildPtr());
        }

        if (targetNode) {
            targetNode->setItem(lastNode->getItem());
            // Delete lastNode from tree
            std::queue<std::shared_ptr<BinaryNode<ItemType>>> delQueue;
            delQueue.push(rootPtr);

            while (!delQueue.empty()) {
                auto current = delQueue.front();
                delQueue.pop();

                if (current->getLeftChildPtr() == lastNode) {
                    current->setLeftChildPtr(nullptr);
                    break;
                }
                else if (current->getRightChildPtr() == lastNode) {
                    current->setRightChildPtr(nullptr);
                    break;
                }

                if (current->getLeftChildPtr())
                    delQueue.push(current->getLeftChildPtr());
                if (current->getRightChildPtr())
                    delQueue.push(current->getRightChildPtr());
            }

            removed = true;
        }

        return removed;
    }

    void clear() override {
        rootPtr = nullptr;
    }

    bool contains(const ItemType& anEntry) const override {
        return findNodeInTree(rootPtr, anEntry) != nullptr;
    }

    //------------------------------------------------------------
    // Public Traversals Section.
    //------------------------------------------------------------
    void preorderTraverse(std::function<void(ItemType&)> visit) const override {
        preorder(visit, rootPtr);
    }
    void inorderTraverse(std::function<void(ItemType&)> visit) const override {
        inorder(visit, rootPtr);
    }
    void postorderTraverse(std::function<void(ItemType&)> visit) const override {
        postorder(visit, rootPtr);
    }

    //------------------------------------------------------------
    // Overloaded Operator Section.
    //------------------------------------------------------------
    BinaryNodeTree& operator=(const BinaryNodeTree& other) {
        if (this == &other) {
            return *this;
        }

        rootPtr = copyTree(other.rootPtr);

        return *this;
    }
}; // end BinaryNodeTree

// *************************** BinarySearchTree ******************************

template<class ItemType>
class BinarySearchTreeInterface
{
public:
    /** Tests whether this binary tree is empty.
     @return  True if the binary tree is empty, or false if not. */
    virtual bool isEmpty() const = 0;

    /** Gets the height of this binary tree.
     @return  The height of the binary tree. */
    virtual int getHeight() const = 0;

    /** Gets the number of nodes in this binary tree.
     @return  The number of nodes in the binary tree. */
    virtual int getNumberOfNodes() const = 0;

    /** Adds the given data to this binary tree.
     @param newData  The data to add to the binary tree.
     @post  The binary tree contains the new data.
     @return  True if the addition is successful, or false if not. */
    virtual bool add(const ItemType& newData) = 0;

    /** Removes the specified data from this binary tree.
     @param target  The data to remove from the binary tree.
     @return  True if the removal is successful, or false if not. */
    virtual bool remove(const ItemType& target) = 0;

    /** Removes all data from this binary tree. */
    virtual void clear() = 0;

    /** Tests whether the specified data occurs in this binary tree.
     @post  The binary tree is unchanged.
     @param target  The data to find.
     @return  True if data matching the target occurs in the tree, or false if not. */
    virtual bool contains(const ItemType& target) const = 0;

    /** Traverses this binary tree in preorder (inorder, postorder) and
        calls the function visit once for each node.
     @param visit  A client-defined function that performs an operation on
        either each visited node or its data. */
    virtual void preorderTraverse(std::function<void(ItemType&)> visit) const = 0;
    virtual void inorderTraverse(std::function<void(ItemType&)> visit) const = 0;
    virtual void postorderTraverse(std::function<void(ItemType&)> visit) const = 0;

    /** Destroys this tree and frees its assigned memory. */
    virtual ~BinarySearchTreeInterface() = default;
}; // end BinarySearchTreeInterface

template<class ItemType>
class BinarySearchTree final : public BinarySearchTreeInterface<ItemType>
{
private:
    // Here we are using composition and a more from scratch implementation instead of inheriting
    // from BinaryNodeTree directly. This gives us more control over the interface. For example,
    // we didn't need to include setRootValue in the public interface to the client. You will see
    // both approaches in practice; composition and public inheritance with protected helpers.
    std::shared_ptr<BinaryNode<ItemType>> rootPtr;

    // Places a given new node at its proper position in this binary search tree.
    auto placeNode(std::shared_ptr<BinaryNode<ItemType>> subTreePtr, std::shared_ptr<BinaryNode<ItemType>> newNodePtr) {
        if (subTreePtr == nullptr) {
            return newNodePtr;
        }
        else if (subTreePtr->getItem() > newNodePtr->getItem()) {
            auto tmpPtr = placeNode(subTreePtr->getLeftChildPtr(), newNodePtr);
            subTreePtr->setLeftChildPtr(tmpPtr);
        }
        else {
            auto tmpPtr = placeNode(subTreePtr->getRightChildPtr(), newNodePtr);
            subTreePtr->setRightChildPtr(tmpPtr);
        }
        return subTreePtr;
    }

    // Note we are NOT overriding this from the base class as the text suggests, preferring to use containment instead.
    // Removes the given target value from the tree while maintaining a binary search tree.
    std::shared_ptr<BinaryNode<ItemType>> removeValue(std::shared_ptr<BinaryNode<ItemType>> subTreePtr,
                                                      const ItemType target, bool& isSuccessful) {
        if (subTreePtr == nullptr) {
            isSuccessful = false;
        }
        else if (subTreePtr->getItem() == target) {
            subTreePtr = removeNode(subTreePtr);
            isSuccessful = true;
        }
        else if (subTreePtr->getItem() > target) {
            auto tmpPtr = removeValue(subTreePtr->getLeftChildPtr(), target, isSuccessful);
            subTreePtr->setLeftChildPtr(tmpPtr);
        }
        else {
            auto tmpPtr = removeValue(subTreePtr->getRightChildPtr(), target, isSuccessful);
            subTreePtr->setRightChildPtr((tmpPtr));
        }

        return subTreePtr;
    }

    // REF: https://www.geeksforgeeks.org/deletion-in-binary-search-tree/
    //
    // Hint: In order to delete a leaf node, we return nullptr. Because we are using smart pointers it will lose
    //       all it's references and clean itself up.
    //
    // Removes a given node from a tree while maintaining a binary search tree.
    std::shared_ptr<BinaryNode<ItemType>> removeNode(std::shared_ptr<BinaryNode<ItemType>> node)
    {
        // TODO: Handle three cases for BST removal:
        // Case 1: Node is a leaf (no children)
        //         -> Return nullptr (smart_ptr will auto-cleanup)
        // Case 2: Node has one child
        //         -> Return the single child to replace this node
        // Case 3: Node has two children
        //         -> Find inorder successor (use findInOrderSuccessor helper)
        //         -> Replace node's data with successor's data
        //         -> Recursively remove successor from right subtree (using cases 1 & 2)
        if (node->isLeaf()) {
            return nullptr;}
        else if (node->getLeftChildPtr() == nullptr) {
            return node->getRightChildPtr();}
        else if (node->getRightChildPtr() == nullptr) {
            return node->getLeftChildPtr();}
        else {
            auto successor = findInOrderSuccessor(node);
            node->setItem(successor->getItem());

            bool dummy;
            node->setRightChildPtr(removeValue(node->getRightChildPtr(), successor->getItem(), dummy));

            return node;
        }
    } // end removeNode

public:
    //------------------------------------------------------------
    // Constructor and Destructor Section.
    //------------------------------------------------------------
    BinarySearchTree() : rootPtr(nullptr)
    {
    }  // end default constructor

    explicit BinarySearchTree(const ItemType& rootItem) : rootPtr(std::make_shared<BinaryNode<ItemType>>(rootItem, nullptr, nullptr))
    {
    } // end constructor

    BinarySearchTree(const ItemType& rootItem,
                   const std::shared_ptr<BinaryNodeTree<ItemType>> leftTreePtr,
                   const std::shared_ptr<BinaryNodeTree<ItemType>> rightTreePtr)
            : rootPtr(std::make_shared<BinaryNode<ItemType>>(rootItem,
                                                             copyTree(leftTreePtr->rootPtr),
                                                             copyTree(rightTreePtr->rootPtr)))
    {
    } // end constructor

    BinarySearchTree(const BinaryNodeTree<ItemType>& treePtr)
    {
        rootPtr = copyTree(treePtr.rootPtr);
    }  // end copy constructor

    ~BinarySearchTree() override = default;

    //------------------------------------------------------------
    // Public Methods Section.
    //------------------------------------------------------------
    // This add maintains the BST ordering using placeNode.
    bool add(const ItemType& newData) override {
        auto newNodePtr = std::make_shared<BinaryNode<ItemType>>(newData);
        rootPtr = placeNode(rootPtr, newNodePtr);
        return true;
    }

    //------------------------------------------------------------
    // Overloaded Operator Section.
    //------------------------------------------------------------
    BinarySearchTree<ItemType>&
    operator=(const BinarySearchTree<ItemType>& other) {
        if (this == &other) {
            return *this;
        }

        rootPtr = copyTree(other.rootPtr);

        return *this;
    }

    bool isEmpty() const override {
        return rootPtr == nullptr;
    }

    int getHeight() const override {
        return heightAtTree(rootPtr);
    }

    int getNumberOfNodes() const override {
        return nodeCountAtTree(rootPtr);
    }

    bool remove(const ItemType &target) override {
        bool removed;
        rootPtr = removeValue(rootPtr, target, removed);
        return removed;
    }

    void clear() override {
        rootPtr = nullptr;
    }

    bool contains(const ItemType &target) const override {

        // TODO: Replace this find with one for the BST that performs the binary search algorithm to
        //       find the node in O(log n) time instead of O(n) as is done int the BNT.
        auto current = rootPtr;
        while (current) {
            if (target == current->getItem())
                return true;
            else if (target < current->getItem())
                current = current->getLeftChildPtr();
            else
                current = current->getRightChildPtr();
        }
        return false;
    }

    void preorderTraverse(std::function<void(ItemType &)> visit) const override {
        preorder(visit, rootPtr);
    }

    void inorderTraverse(std::function<void(ItemType &)> visit) const override {
        inorder(visit, rootPtr);
    }

    void postorderTraverse(std::function<void(ItemType &)> visit) const override {
        postorder(visit, rootPtr);
    }
}; // end BinarySearchTree

// *************************** treeSort *******************************

// REF: https://www.programiz.com/cpp-programming/lambda-expression
template<class ItemType>
void treeSort(ItemType array_in[], int array_size) {
    BinarySearchTree<ItemType> bst;

    // TODO:
    // Step 1: Add all array elements to BST
    // Step 2: Use inorder traversal with lambda to extract sorted values back to array
    for (int i = 0; i < array_size; ++i) {
        bst.add(array_in[i]);
    }

    // Step 2: Extract sorted elements using in-order traversal
    int index = 0;
    bst.inorderTraverse([&](ItemType& val) {
        array_in[index++] = val;
    });
}

// *************************** Unit Tests ******************************

TEST_CASE("testing unordered binary tree with balanced add and removal") {
    BinaryNodeTree<int> tree;

    SUBCASE("empty tree tests") {
        CHECK(tree.isEmpty());
        CHECK_EQ(tree.getHeight(), -1);
        CHECK_EQ(tree.getNumberOfNodes(), 0);
        CHECK_FALSE(tree.contains(5));
        CHECK_THROWS(tree.getRootData());
    }

    SUBCASE("single node tree") {
        tree.add(10);
        CHECK_FALSE(tree.isEmpty());
        CHECK_EQ(tree.getHeight(), 0);
        CHECK_EQ(tree.getNumberOfNodes(), 1);
        CHECK_EQ(tree.getRootData(), 10);
        CHECK(tree.contains(10));
        CHECK_FALSE(tree.contains(5));
    }

    SUBCASE("multiple node additions") {
        tree.add(10);
        tree.add(5);
        tree.add(15);
        tree.add(3);
        tree.add(7);

        CHECK_EQ(tree.getNumberOfNodes(), 5);
        CHECK(tree.contains(10));
        CHECK(tree.contains(5));
        CHECK(tree.contains(15));
        CHECK(tree.contains(3));
        CHECK(tree.contains(7));
        CHECK_FALSE(tree.contains(20));
    }

    SUBCASE("setRootData functionality") {
        // Test setting root data on empty tree
        tree.setRootData(42);
        CHECK_FALSE(tree.isEmpty());
        CHECK_EQ(tree.getRootData(), 42);
        CHECK_EQ(tree.getNumberOfNodes(), 1);

        // Test replacing root data on non-empty tree
        tree.setRootData(99);
        CHECK_EQ(tree.getRootData(), 99);
        CHECK_EQ(tree.getNumberOfNodes(), 1);
    }

    SUBCASE("copy constructor") {
        // Build original tree
        tree.add(10);
        tree.add(5);
        tree.add(15);
        tree.add(3);

        // Create copy
        BinaryNodeTree<int> treeCopy(tree);

        // Verify copy has same structure and data
        CHECK_EQ(treeCopy.getNumberOfNodes(), tree.getNumberOfNodes());
        CHECK_EQ(treeCopy.getHeight(), tree.getHeight());
        CHECK_EQ(treeCopy.getRootData(), tree.getRootData());

        // Verify all elements are present in copy
        CHECK(treeCopy.contains(10));
        CHECK(treeCopy.contains(5));
        CHECK(treeCopy.contains(15));
        CHECK(treeCopy.contains(3));

        // Verify independence - modify original
        tree.add(20);
        CHECK_EQ(tree.getNumberOfNodes(), 5);
        CHECK_EQ(treeCopy.getNumberOfNodes(), 4);
        CHECK(tree.contains(20));
        CHECK_FALSE(treeCopy.contains(20));
    }

    SUBCASE("assignment operator") {
        // Build original tree
        tree.add(10);
        tree.add(5);
        tree.add(15);

        // Create another tree and assign
        BinaryNodeTree<int> tree2;
        tree2.add(99);  // Give it some data first
        tree2 = tree;

        // Verify assignment worked
        CHECK_EQ(tree2.getNumberOfNodes(), tree.getNumberOfNodes());
        CHECK_EQ(tree2.getRootData(), tree.getRootData());
        CHECK(tree2.contains(10));
        CHECK(tree2.contains(5));
        CHECK(tree2.contains(15));
        CHECK_FALSE(tree2.contains(99));  // Old data should be gone

        // Test self-assignment
        tree2 = tree2;
        CHECK_EQ(tree2.getNumberOfNodes(), 3);
        CHECK(tree2.contains(10));
    }

    SUBCASE("remove functionality") {
        // Build tree
        tree.add(10);
        tree.add(5);
        tree.add(15);
        tree.add(3);
        tree.add(7);

        // Test removing existing elements
        CHECK(tree.remove(7));
        CHECK_EQ(tree.getNumberOfNodes(), 4);
        CHECK_FALSE(tree.contains(7));
        CHECK(tree.contains(10));  // Other elements still there
        CHECK(tree.contains(5));
        CHECK(tree.contains(15));
        CHECK(tree.contains(3));

        // Test removing root
        CHECK(tree.remove(10));
        CHECK_EQ(tree.getNumberOfNodes(), 3);
        CHECK_FALSE(tree.contains(10));

        // Test removing non-existent element
        CHECK_FALSE(tree.remove(99));
        CHECK_EQ(tree.getNumberOfNodes(), 3);

        // Remove all remaining elements
        CHECK(tree.remove(5));
        CHECK(tree.remove(15));
        CHECK(tree.remove(3));
        CHECK_FALSE(tree.isEmpty());
    }

    SUBCASE("clear functionality") {
        tree.add(10);
        tree.add(5);
        tree.add(15);

        tree.clear();
        CHECK(tree.isEmpty());
        CHECK_EQ(tree.getNumberOfNodes(), 0);
        CHECK_EQ(tree.getHeight(), -1);
        CHECK_FALSE(tree.contains(10));
    }

    SUBCASE("traversal functionality") {
        tree.add(10);
        tree.add(5);
        tree.add(15);
        tree.add(3);
        tree.add(7);

        std::vector<int> preorderResult;
        std::vector<int> inorderResult;
        std::vector<int> postorderResult;

        auto collectValues = [](std::vector<int>& vec) {
            return [&vec](int& value) { vec.push_back(value); };
        };

        tree.preorderTraverse(collectValues(preorderResult));
        tree.inorderTraverse(collectValues(inorderResult));
        tree.postorderTraverse(collectValues(postorderResult));

        // Verify we got all elements in each traversal
        CHECK_EQ(preorderResult.size(), 5);
        CHECK_EQ(inorderResult.size(), 5);
        CHECK_EQ(postorderResult.size(), 5);

        // Check that all expected values are present (order depends on balanced add implementation)
        std::vector<int> expected = {10, 5, 15, 3, 7};
        std::sort(expected.begin(), expected.end());

        std::sort(preorderResult.begin(), preorderResult.end());
        std::sort(inorderResult.begin(), inorderResult.end());
        std::sort(postorderResult.begin(), postorderResult.end());

        CHECK_EQ(preorderResult, expected);
        CHECK_EQ(inorderResult, expected);
        CHECK_EQ(postorderResult, expected);
    }
}

TEST_CASE("testing ordered binary search tree with add and removal that maintain ordering") {
    BinarySearchTree<int> bst;

    SUBCASE("empty BST tests") {
        CHECK(bst.isEmpty());
        CHECK_EQ(bst.getHeight(), -1);
        CHECK_EQ(bst.getNumberOfNodes(), 0);
        CHECK_FALSE(bst.contains(5));
    }

    SUBCASE("single node BST") {
        bst.add(10);
        CHECK_FALSE(bst.isEmpty());
        CHECK_EQ(bst.getHeight(), 0);
        CHECK_EQ(bst.getNumberOfNodes(), 1);
        CHECK(bst.contains(10));
        CHECK_FALSE(bst.contains(5));
    }

    SUBCASE("BST ordering property") {
        bst.add(10);
        bst.add(5);
        bst.add(15);
        bst.add(3);
        bst.add(7);
        bst.add(12);
        bst.add(18);

        CHECK_EQ(bst.getNumberOfNodes(), 7);

        // Verify all elements are present
        CHECK(bst.contains(10));
        CHECK(bst.contains(5));
        CHECK(bst.contains(15));
        CHECK(bst.contains(3));
        CHECK(bst.contains(7));
        CHECK(bst.contains(12));
        CHECK(bst.contains(18));
        CHECK_FALSE(bst.contains(99));

        // Verify inorder traversal gives sorted order
        std::vector<int> inorderResult;
        bst.inorderTraverse([&inorderResult](int& value) {
            inorderResult.push_back(value);
        });

        std::vector<int> expected = {3, 5, 7, 10, 12, 15, 18};
        CHECK_EQ(inorderResult, expected);
    }

    SUBCASE("BST with duplicate values") {
        bst.add(10);
        bst.add(10);  // Duplicate
        bst.add(5);
        bst.add(10);  // Another duplicate

        // Should handle duplicates gracefully (typically go right)
        CHECK(bst.contains(10));
        CHECK(bst.contains(5));
        CHECK_GE(bst.getNumberOfNodes(), 3);  // At least 3 nodes
    }

    SUBCASE("copy constructor for BST") {
        // Build original BST
        bst.add(10);
        bst.add(5);
        bst.add(15);
        bst.add(3);
        bst.add(7);

        // Create copy
        BinarySearchTree<int> bstCopy(bst);

        // Verify copy has same structure and maintains BST property
        CHECK_EQ(bstCopy.getNumberOfNodes(), bst.getNumberOfNodes());
        CHECK_EQ(bstCopy.getHeight(), bst.getHeight());

        // Verify BST ordering is maintained in copy
        std::vector<int> originalOrder;
        std::vector<int> copyOrder;

        bst.inorderTraverse([&originalOrder](int& value) {
            originalOrder.push_back(value);
        });

        bstCopy.inorderTraverse([&copyOrder](int& value) {
            copyOrder.push_back(value);
        });

        CHECK_EQ(originalOrder, copyOrder);

        // Verify independence
        bst.add(20);
        CHECK(bst.contains(20));
        CHECK(bstCopy.contains(20));
    }

    SUBCASE("assignment operator for BST") {
        // Build original BST
        bst.add(10);
        bst.add(5);
        bst.add(15);

        // Create another BST and assign
        BinarySearchTree<int> bst2;
        bst2.add(99);
        bst2 = bst;

        // Verify assignment maintains BST property
        std::vector<int> bst1Order;
        std::vector<int> bst2Order;

        bst.inorderTraverse([&bst1Order](int& value) {
            bst1Order.push_back(value);
        });

        bst2.inorderTraverse([&bst2Order](int& value) {
            bst2Order.push_back(value);
        });

        CHECK_EQ(bst1Order, bst2Order);
        CHECK_FALSE(bst2.contains(99));  // Old data should be gone
    }

    SUBCASE("BST remove functionality") {
        // Build BST
        bst.add(10);
        bst.add(5);
        bst.add(15);
        bst.add(3);
        bst.add(7);
        bst.add(12);
        bst.add(18);

        // Test removing leaf node
        CHECK(bst.remove(3));
        CHECK_EQ(bst.getNumberOfNodes(), 6);
        CHECK_FALSE(bst.contains(3));

        // Verify BST property maintained after removal
        std::vector<int> inorderAfterLeafRemoval;
        bst.inorderTraverse([&inorderAfterLeafRemoval](int& value) {
            inorderAfterLeafRemoval.push_back(value);
        });
        std::vector<int> expectedAfterLeaf = {5, 7, 10, 12, 15, 18};
        CHECK_EQ(inorderAfterLeafRemoval, expectedAfterLeaf);

        // Test removing node with one child
        CHECK(bst.remove(18));
        CHECK_EQ(bst.getNumberOfNodes(), 5);
        CHECK_FALSE(bst.contains(18));

        // Test removing node with two children (root)
        CHECK(bst.remove(10));
        CHECK_EQ(bst.getNumberOfNodes(), 4);
        CHECK_FALSE(bst.contains(10));

        // Verify BST property still maintained
        std::vector<int> finalOrder;
        bst.inorderTraverse([&finalOrder](int& value) {
            finalOrder.push_back(value);
        });
        std::vector<int> expectedFinal = {5, 7, 12, 15};
        CHECK_EQ(finalOrder, expectedFinal);

        // Test removing non-existent element
        CHECK_FALSE(bst.remove(99));
        CHECK_EQ(bst.getNumberOfNodes(), 4);

        // Remove all remaining elements
        CHECK(bst.remove(5));
        CHECK(bst.remove(7));
        CHECK(bst.remove(12));
        CHECK(bst.remove(15));
        CHECK(bst.isEmpty());
    }

    SUBCASE("clear functionality for BST") {
        bst.add(10);
        bst.add(5);
        bst.add(15);

        bst.clear();
        CHECK(bst.isEmpty());
        CHECK_EQ(bst.getNumberOfNodes(), 0);
        CHECK_EQ(bst.getHeight(), -1);
        CHECK_FALSE(bst.contains(10));
    }

    SUBCASE("traversal ordering in BST") {
        bst.add(10);
        bst.add(5);
        bst.add(15);
        bst.add(3);
        bst.add(7);
        bst.add(12);
        bst.add(18);

        std::vector<int> preorderResult;
        std::vector<int> inorderResult;
        std::vector<int> postorderResult;

        bst.preorderTraverse([&preorderResult](int& value) {
            preorderResult.push_back(value);
        });

        bst.inorderTraverse([&inorderResult](int& value) {
            inorderResult.push_back(value);
        });

        bst.postorderTraverse([&postorderResult](int& value) {
            postorderResult.push_back(value);
        });

        // Inorder should be sorted
        std::vector<int> expectedInorder = {3, 5, 7, 10, 12, 15, 18};
        CHECK_EQ(inorderResult, expectedInorder);

        // Preorder should start with root
        CHECK_EQ(preorderResult[0], 10);

        // All traversals should have same number of elements
        CHECK_EQ(preorderResult.size(), 7);
        CHECK_EQ(inorderResult.size(), 7);
        CHECK_EQ(postorderResult.size(), 7);
    }

    SUBCASE("stress test - larger BST") {
        // Add many elements
        std::vector<int> values = {50, 25, 75, 12, 37, 62, 87, 6, 18, 31, 43, 56, 68, 81, 93};

        for (int val : values) {
            bst.add(val);
        }

        CHECK_EQ(bst.getNumberOfNodes(), values.size());

        // Verify all elements are present
        for (int val : values) {
            CHECK(bst.contains(val));
        }

        // Verify inorder traversal is sorted
        std::vector<int> inorderResult;
        bst.inorderTraverse([&inorderResult](int& value) {
            inorderResult.push_back(value);
        });

        std::vector<int> sortedValues = values;
        std::sort(sortedValues.begin(), sortedValues.end());
        CHECK_EQ(inorderResult, sortedValues);

        // Remove half the elements
        for (size_t i = 0; i < values.size() / 2; ++i) {
            CHECK(bst.remove(values[i]));
        }

        // Verify remaining elements still maintain BST property
        std::vector<int> remainingOrder;
        bst.inorderTraverse([&remainingOrder](int& value) {
            remainingOrder.push_back(value);
        });

        // Should still be sorted
        CHECK(std::is_sorted(remainingOrder.begin(), remainingOrder.end()));
    }
}

TEST_CASE("testing in-place tree sort") {
    const int test_array_size = 4;
    int test_array[] = {4, 3, 2, 1};
    treeSort(test_array, test_array_size);
    CHECK_EQ(test_array[0], 1);
    CHECK_EQ(test_array[1], 2);
    CHECK_EQ(test_array[2], 3);
    CHECK_EQ(test_array[3], 4);
}

TEST_CASE("testing the height of a tree") {
    BinaryNodeTree<int> tree;
    assert(tree.getHeight() == -1);

    tree.add(1);
    assert(tree.getHeight() == 0);

    tree.add(2);
    tree.add(3);
    assert(tree.getHeight() == 1);
}

TEST_CASE("testing the copy constructor") {
    BinaryNodeTree<int> tree;
    tree.add(10);
    tree.add(20);
    tree.add(30);

    BinaryNodeTree<int> copiedTree(tree);
    assert(copiedTree.contains(10));
    assert(copiedTree.contains(20));
    assert(copiedTree.contains(30));
}

TEST_CASE("testing balanced add") {
    BinaryNodeTree<int> tree;
    tree.add(1);
    tree.add(2);
    tree.add(3);
    tree.add(4);
    tree.add(5);
    assert(tree.getNumberOfNodes() == 5);
}

TEST_CASE("testing if leaf") {
    BinaryNode<int> node(5);
    assert(node.isLeaf());

    auto left = std::make_shared<BinaryNode<int>>(3);
    node.setLeftChildPtr(left);
    assert(!node.isLeaf());
}

TEST_CASE("testing bst ordering") {
    BinarySearchTree<int> bst;
    bst.add(8);
    bst.add(3);
    bst.add(10);
    bst.add(1);
    bst.add(6);

    std::vector<int> inorder;
    bst.inorderTraverse([&](int& val) { inorder.push_back(val); });

    assert((inorder == std::vector<int>{1, 3, 6, 8, 10}));
}

TEST_CASE("testing bst remove leaf") {
    BinarySearchTree<int> bst;
    bst.add(5);
    bst.add(3);
    bst.add(7);

    assert(bst.remove(3));  // remove leaf
    assert(!bst.contains(3));
}

TEST_CASE("testing bst remove internal") {
    BinarySearchTree<int> bst;
    bst.add(8);
    bst.add(3);
    bst.add(10);
    bst.add(1);
    bst.add(6);

    assert(bst.remove(3));  // internal node
    assert(!bst.contains(3));
}

TEST_CASE("testing tree sort") {
    int array[] = {9, 4, 6, 2, 7};
    treeSort(array, 5);
    assert((array[0] == 2 && array[1] == 4 && array[2] == 6 &&
            array[3] == 7 && array[4] == 9));
}