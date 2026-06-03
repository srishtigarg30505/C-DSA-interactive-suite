#include "data_structures.h"
#include "safe_input.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Returns the height of a given node.
 * An empty node has a height of 0.
 */
int avl_height(const avlNode* node)
{
    if (node == NULL)
        return 0;
    return node->height;
}

/*
 * Helper utility to return the maximum of two integers.
 */
int avl_max(int a, int b)
{
    return (a > b) ? a : b;
}

/*
 * Calculates the balance factor of a node.
 * Balance factor = height(left_subtree) - height(right_subtree)
 */
int avl_balance_factor(const avlNode* node)
{
    if (node == NULL)
        return 0;
    return avl_height(node->left) - avl_height(node->right);
}

/*
 * Performs a Right Rotation (Single rotation) around node y.
 * Used to rebalance a Left-Left (LL) imbalance.
 */
static avlNode* right_rotate(avlNode* y)
{
    avlNode* x = y->left;
    avlNode* T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update node heights
    y->height = avl_max(avl_height(y->left), avl_height(y->right)) + 1;
    x->height = avl_max(avl_height(x->left), avl_height(x->right)) + 1;

    // Return the new root of this subtree
    return x;
}

/*
 * Performs a Left Rotation (Single rotation) around node x.
 * Used to rebalance a Right-Right (RR) imbalance.
 */
static avlNode* left_rotate(avlNode* x)
{
    avlNode* y = x->right;
    avlNode* T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    // Update node heights
    x->height = avl_max(avl_height(x->left), avl_height(x->right)) + 1;
    y->height = avl_max(avl_height(y->left), avl_height(y->right)) + 1;

    // Return the new root of this subtree
    return y;
}

/*
 * Recursive helper to insert a value into the AVL tree.
 * Recalculates subtree heights and triggers rotations if balance factor goes out of limits.
 */
static avlNode* avl_insert_helper(avlNode* node, int value, int* status)
{
    // 1. Perform normal BST insertion
    if (node == NULL)
    {
        avlNode* new_node = malloc(sizeof(avlNode));
        if (new_node == NULL)
        {
            *status = -1; // memory allocation error
            return NULL;
        }
        new_node->data = value;
        new_node->height = 1;
        new_node->left = NULL;
        new_node->right = NULL;
        *status = 1; // successful insertion
        return new_node;
    }

    if (value == node->data)
    {
        *status = 0; // value already exists, abort
        return node;
    }

    if (value < node->data)
    {
        node->left = avl_insert_helper(node->left, value, status);
    }
    else
    {
        node->right = avl_insert_helper(node->right, value, status);
    }

    // If insertion didn't modify the tree (duplicate/malloc error), return unmodified node
    if (*status != 1)
        return node;

    // 2. Update height of this ancestor node
    node->height = avl_max(avl_height(node->left), avl_height(node->right)) + 1;

    // 3. Get the balance factor to check for imbalances
    int balance = avl_balance_factor(node);

    // Left-Left (LL) Case -> Single Right Rotation
    if (balance > 1 && value < node->left->data)
        return right_rotate(node);

    // Right-Right (RR) Case -> Single Left Rotation
    if (balance < -1 && value > node->right->data)
        return left_rotate(node);

    // Left-Right (LR) Case -> Double Rotation (Left then Right)
    if (balance > 1 && value > node->left->data)
    {
        node->left = left_rotate(node->left);
        return right_rotate(node);
    }

    // Right-Left (RL) Case -> Double Rotation (Right then Left)
    if (balance < -1 && value < node->right->data)
    {
        node->right = right_rotate(node->right);
        return left_rotate(node);
    }

    return node;
}

/*
 * Public API to insert a value into the AVL tree.
 * Returns 1 on success, 0 on duplicate, and -1 on memory failure.
 */
int avl_insert(avlNode** root_ref, int value)
{
    int status = 0;
    *root_ref = avl_insert_helper(*root_ref, value, &status);
    return status;
}

/*
 * Recursive helper to delete a node from the AVL tree.
 * Resolves rebalancing after node removal using rotation logic.
 */
static avlNode* avl_delete_helper(avlNode* root, int value, int* status)
{
    // 1. Perform standard BST deletion
    if (root == NULL)
    {
        *status = 0; // value not found
        return NULL;
    }

    if (value < root->data)
    {
        root->left = avl_delete_helper(root->left, value, status);
    }
    else if (value > root->data)
    {
        root->right = avl_delete_helper(root->right, value, status);
    }
    else
    {
        *status = 1; // node found, delete it

        // Case A: Node with only one child or no child
        if ((root->left == NULL) || (root->right == NULL))
        {
            avlNode* temp = root->left ? root->left : root->right;

            if (temp == NULL) // No child
            {
                temp = root;
                root = NULL;
            }
            else // One child
            {
                *root = *temp; // Copy content of non-empty child
            }
            free(temp);
        }
        else // Case B: Node with two children
        {
            // Retrieve inorder successor (smallest in the right subtree)
            avlNode* temp = root->right;
            while (temp->left != NULL)
                temp = temp->left;

            // Copy inorder successor's value
            root->data = temp->data;

            // Delete the successor node
            int dummy_status;
            root->right = avl_delete_helper(root->right, temp->data, &dummy_status);
        }
    }

    if (root == NULL)
        return NULL;

    // 2. Update height of current node
    root->height = avl_max(avl_height(root->left), avl_height(root->right)) + 1;

    // 3. Check balance factor to verify AVL balance properties
    int balance = avl_balance_factor(root);

    // Left-Left (LL) imbalance -> Single Right Rotation
    if (balance > 1 && avl_balance_factor(root->left) >= 0)
        return right_rotate(root);

    // Left-Right (LR) imbalance -> Double Rotation (Left then Right)
    if (balance > 1 && avl_balance_factor(root->left) < 0)
    {
        root->left = left_rotate(root->left);
        return right_rotate(root);
    }

    // Right-Right (RR) imbalance -> Single Left Rotation
    if (balance < -1 && avl_balance_factor(root->right) <= 0)
        return left_rotate(root);

    // Right-Left (RL) imbalance -> Double Rotation (Right then Left)
    if (balance < -1 && avl_balance_factor(root->right) > 0)
    {
        root->right = right_rotate(root->right);
        return left_rotate(root);
    }

    return root;
}

/*
 * Public API to delete a node from the AVL tree.
 * Returns 1 on success, and 0 if the value does not exist.
 */
int avl_delete(avlNode** root_ref, int value)
{
    int status = 0;
    *root_ref = avl_delete_helper(*root_ref, value, &status);
    return status;
}

/*
 * Prints inorder traversal (Left, Root, Right).
 */
void avl_inorder(const avlNode* root)
{
    if (root == NULL)
        return;
    avl_inorder(root->left);
    printf("%d,", root->data);
    avl_inorder(root->right);
}

/*
 * Prints preorder traversal (Root, Left, Right).
 */
void avl_preorder(const avlNode* root)
{
    if (root == NULL)
        return;
    printf("%d,", root->data);
    avl_preorder(root->left);
    avl_preorder(root->right);
}

/*
 * Prints postorder traversal (Left, Right, Root).
 */
void avl_postorder(const avlNode* root)
{
    if (root == NULL)
        return;
    avl_postorder(root->left);
    avl_postorder(root->right);
    printf("%d,", root->data);
}

/*
 * Deallocates all nodes within the AVL tree.
 */
void destroy_avl(avlNode* root)
{
    if (root == NULL)
        return;
    destroy_avl(root->left);
    destroy_avl(root->right);
    free(root);
}

/*
 * CLI demo interface to interact with AVL tree module.
 */
void avl_demo(void)
{
    while (1)
    {
        avlNode* root = NULL;
        int total_nodes;
        int total_nodes_status = safe_input_int(&total_nodes,
                                                "\n\nenter total number of nodes you want in the AVL tree, "
                                                "(between 1 and 100), enter '-1' to exit:- ",
                                                1, 100);

        if (total_nodes_status == INPUT_EXIT_SIGNAL)
        {
            printf("\nExiting AVL tree demo\n");
            destroy_avl(root);
            return;
        }
        if (total_nodes_status == 0)
        {
            continue;
        }

        int i = 1;
        while (total_nodes > 0)
        {
            int node_value;
            printf("\nenter value of %d AVL node - ", i);
            int node_value_status = safe_input_int(&node_value, NULL, 1, 100);

            if (node_value_status == INPUT_EXIT_SIGNAL)
            {
                printf("\nExiting AVL tree demo\n");
                destroy_avl(root);
                return;
            }
            if (node_value_status == 0)
            {
                continue;
            }

            int insertion_status = avl_insert(&root, node_value);
            if (insertion_status == 0)
            {
                printf("\nentered same value. only unique values please");
                continue;
            }
            if (insertion_status == -1)
            {
                printf("\ncouldnt insert node due to malloc failure. try again\n");
                continue;
            }
            i++;
            total_nodes--;
        }

        printf("\nheight of the AVL tree is:- %d\n", avl_height(root));

        while (1)
        {
            int traversal_choice;
            int traversal_status = safe_input_int(&traversal_choice,
                                                  "\nenter '1' for inorder, '2' for preorder and "
                                                  "'3' for postorder, '4' to delete a node, '5' to check balance factor, and '-1' to exit:- ",
                                                  1, 5);

            if (traversal_status == INPUT_EXIT_SIGNAL)
            {
                printf("\nExiting AVL tree demo\n");
                destroy_avl(root);
                return;
            }
            if (traversal_status == 0)
            {
                continue;
            }

            if (traversal_choice == 1)
            {
                avl_inorder(root);
                printf("\n");
            }
            else if (traversal_choice == 2)
            {
                avl_preorder(root);
                printf("\n");
            }
            else if (traversal_choice == 3)
            {
                avl_postorder(root);
                printf("\n");
            }
            else if (traversal_choice == 4)
            {
                int delete_value;
                int delete_status;
                while (1)
                {
                    delete_status = safe_input_int(&delete_value,
                                                   "\nenter value to delete (between 1 and 100), enter '-1' to exit:- ",
                                                   1, 100);
                    if (delete_status == INPUT_EXIT_SIGNAL)
                    {
                        printf("\nExiting AVL tree demo\n");
                        destroy_avl(root);
                        return;
                    }
                    if (delete_status == 0)
                        continue;
                    break;
                }
                int status = avl_delete(&root, delete_value);
                if (status == 0)
                {
                    printf("\nvalue not found in the tree\n");
                }
                else
                {
                    printf("\nnode deleted. updated inorder traversal: ");
                    avl_inorder(root);
                    printf("\n");
                }
            }
            else if (traversal_choice == 5)
            {
                printf("\nbalance factor of root node is: %d\n", avl_balance_factor(root));
            }
        }
    }
}
