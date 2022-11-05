#include "equal-paths.h"

// You may add any prototypes of helper functions here
int equalPaths_helper (Node* parent);

bool equalPaths(Node * root)
{
    // Add your code below
    if (root == nullptr) {
        return true;
    }

    return (equalPaths_helper(root->left) == equalPaths_helper(root->right));
}

int equalPaths_helper (Node* parent) {
    if (parent == nullptr) {
        return 0;
    }

    if (parent->left && parent->right) {
        return max(equalPaths_helper(parent->left), equalPaths_helper(parent->right)) + 1;
    }
    else if (parent->left) {
        return equalPaths_helper(parent->left) + 1;
    }
    else if (parent->right) {
        return equalPaths_helper(parent->right) + 1;
    }
    return 1;
}
