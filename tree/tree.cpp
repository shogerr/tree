#include "tree.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>


#pragma region qnode tree


//  process one node
bool qtree::process()
{
    auto currentNode = nodeQueue.top();
    nodeQueue.pop();

    if (!isViable(currentNode))
        return false;

    addNode(currentNode);

    // create a child node for each available transform.
    // all child nodes are added to the queue, even if not viable.
    for (auto const &t : transforms)
    {
        qnode child;
        beget(currentNode, t, child);
        assert(currentNode.id == 0 || currentNode.parentId < currentNode.id);
        nodeQueue.push(child);
    }

    return true;
}


void qtree::addNode(qnode & node)
{
    if(!node.sourceTransform.empty())
        transformCounts[node.sourceTransform]++;
}


//  Generate a potential child node from parent
void qtree::beget(qnode const & parent, qtransform const & t, qnode & child)
{
    child.id = nextNodeId++;
    child.parentId = parent.id;
    child.sourceTransform = t.transformMatrixKey;

    child.beginTime = parent.beginTime + t.gestation + (gestationRandomness>0.0 ? r(gestationRandomness) : 0.0);

    child.globalTransform = parent.globalTransform * t.transformMatrix;

    child.color = t.colorTransform.apply(parent.color);
}


//  Node draw function for tree of nodes with all the same polygon
void qtree::drawNode(qcanvas &canvas, qnode const &node)
{
    cv::Scalar color =
        //(node.det() < 0) ? 255.0 * (cv::Scalar(1.0, 1.0, 1.0, 1.0) - node.color) : 
        255.0 * node.color;

    canvas.fillPoly(polygon, node.globalTransform, 255.0 * node.color, lineThickness, lineColor);
}


#pragma endregion

