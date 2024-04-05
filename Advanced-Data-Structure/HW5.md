# HW5

### 红黑树实现

fixViolation函数的实现：

要求实现部分为叔叔为黑色的情况，分别操作即可。

```cpp
// L and Uncle is black
	// LL-Case and LR-Case, write your code here
    // LL-Case
    if (pt == parent_pt->left) {
    	rotateRight(grand_parent_pt);
        grand_parent_pt->color = RED;
        parent_pt->color = BLACK;
        pt = parent_pt;
    }
    // LR-case
    else {
    	rotateLeft(parent_pt);
	    rotateRight(grand_parent_pt);
    	pt->color = BLACK;
    	grand_parent_pt->color = RED;
    }


// R and Uncle is black
	// RR-Case and RL-Case, write your code herehere
	// RL-case
    if (pt == parent_pt->left) {
        rotateRight(parent_pt);
        rotateLeft(grand_parent_pt);
        pt->color = BLACK;
    	grand_parent_pt->color = RED;
	}
    else {
        rotateLeft(grand_parent_pt);
        grand_parent_pt->color = RED;
        parent_pt->color = BLACK;
    	pt = parent_pt;
    }
```



### 问题问答

1. 代码正确性

   1. 插入后的红黑树：![AE3E93E1019300A5E801A135984D511E](E:\TencentQQ\1229168058\FileRecv\MobileFile\AE3E93E1019300A5E801A135984D511E.png)

   2. inorder输出结果：![image-20240404175852732](C:\Users\LEGION\AppData\Roaming\Typora\typora-user-images\image-20240404175852732.png)

      与手动画出来的树保持一致。

2. 删除后如何维持性质：

   1. 若要删除的结点有两个孩子，则取前驱（或后继）结点替换他，但是不改变各颜色和关系，而后删除前驱（或后继）结点即可。性质待后续调整。
   2. 待删除的结点为叶子结点，若为红色，直接删除，无影响。若为黑色，要重新维护。
   3. 若删除节点只有一个孩子（非外部节点），则其孩子必为红色（否则违反性质5，高度不同），将孩子提上来并染黑（保持黑高，维持性质5）。
   4. 对黑高失衡的情况，做修正（在这里，本节点为出现了黑高降低的结点，可以是递归上来的）：
      1. 若父节点为黑，兄弟结点为红色，左旋父节点并变为红色，将兄弟结点提上去变成黑色，使个节点的黑高不变，维持了性质5.
      2. 若父节点为红，兄弟结点和侄子节点都是黑色，则染色父结点为黑，兄弟节点为红即可。
      3. 若父节点、兄弟节点、侄子节点全是黑的，则将兄弟染红，保持局部性质满足，向上递归。（局部满足性质5，但以父结点为根的子树黑高降低了。
      4. 若兄弟结点为黑色，侄子节点靠近一侧为红，则做一次旋转，将红色侄子节点变为兄弟子树的根，染黑，原兄弟结点染红下放，按第五种情况处理。
      5. 若兄弟为黑，侄子远离一侧为红，则做一次旋转，将兄弟转上来，父节点转下去，并将原来红色的侄子染黑，性质维护完成。

3. 顺序插入和乱序插入的影响：

   1. 统计结果：

      ![image-20240405155945653](C:\Users\LEGION\AppData\Roaming\Typora\typora-user-images\image-20240405155945653.png)

   2. 两种插入方式的影响：

      从实验结果中可以看到，当输入数据是顺序时，失衡次数、旋转次数以及重新染色的次数都大约是乱序情况下的两倍。而这也以为着红黑树在面对乱序输入时的表现会更好。同时，逆序插入与顺序插入的表现完全一致，说明红黑树有高度的对称性。在我看来，出现这种情况的主要原因有以下几点：

      1. 顺序插入会导致树的高度增加更快，因为顺序插入每次都在同一边会使红黑树的黑高虽然一致，但是右边的总高度会更高，黑色结点都会集中到红黑树的左侧。
      2. 顺序插入导致最右侧的链基本上为一个红色一个黑色，而每次插入时要查询的结点都在最右侧的链上，导致查询的时间也会增加。
      3. 由于黑色结点集中到了树的左侧，但没有小的数值插入到左侧，因此少了很多可以直接插入的红色节点，也就提高了失衡次数、旋转次数和重新染色次数。