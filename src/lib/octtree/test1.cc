#include <iostream>
using std::cout;
using std::endl;

/// \file octtree/test.cc

#include <fstream>

//#include <mra/mra.h>
#include <misc/misc.h>
#include <octtree/sendrecv.h>
//#include <octtree/octtree.h>
using namespace madness;
    

typedef double T;

int main (int argc, char **argv) {
    
    MPI::Init(argc, argv);
    Communicator comm;

    redirectio(comm);
    //xterm_debug(comm,0,0);
    
    comm.print();

    int me = comm.rank();
    int nproc = comm.nproc();
    ProcessID rank = comm.rank();


    if (me == 0)
    {
	/********************************************************************************/
	/*										*/
	/* Initialize tree to be partitioned						*/
	/*										*/
	/********************************************************************************/

	std::vector< std::vector< RootList > > pieces;
	std::vector< RootList > list;
	OctTree<T> *t = new OctTree<T>(0, 0,0,0, false, 0, -1, &comm);
        cout << "t.getCost() = " << t->getCost() << endl;

	t->setData(3.14159);
	
        OctTree<T>* child0 = t->insert_local_child(0,0,0);
        OctTree<T>* child1 = t->insert_local_child(0,0,1);
        OctTree<T>* child2 = t->insert_local_child(0,1,0);
        OctTree<T>* child3 = t->insert_local_child(0,1,1);
        OctTree<T>* child4 = t->insert_local_child(1,0,0);
        OctTree<T>* child5 = t->insert_local_child(1,0,1);
        OctTree<T>* child6 = t->insert_local_child(1,1,0);
        OctTree<T>* child7 = t->insert_local_child(1,1,1);

        OctTree<T>* child30 = child3->insert_local_child(0,0,0);
        OctTree<T>* child31 = child3->insert_local_child(0,0,1);
        OctTree<T>* child32 = child3->insert_local_child(0,1,0);
        OctTree<T>* child33 = child3->insert_local_child(0,1,1);
        OctTree<T>* child34 = child3->insert_local_child(1,0,0);
        OctTree<T>* child35 = child3->insert_local_child(1,0,1);
        OctTree<T>* child36 = child3->insert_local_child(1,1,0);
        OctTree<T>* child37 = child3->insert_local_child(1,1,1);

        OctTree<T>* child370 = child37->insert_local_child(0,0,0);
        OctTree<T>* child371 = child37->insert_local_child(0,0,1);
        OctTree<T>* child372 = child37->insert_local_child(0,1,0);
        OctTree<T>* child373 = child37->insert_local_child(0,1,1);
        OctTree<T>* child374 = child37->insert_local_child(1,0,0);
        OctTree<T>* child375 = child37->insert_local_child(1,0,1);
        OctTree<T>* child376 = child37->insert_local_child(1,1,0);
        OctTree<T>* child377 = child37->insert_local_child(1,1,1);

        OctTree<T>* child40 = child4->insert_local_child(0,0,0);
        OctTree<T>* child41 = child4->insert_local_child(0,0,1);
        OctTree<T>* child42 = child4->insert_local_child(0,1,0);
        OctTree<T>* child43 = child4->insert_local_child(0,1,1);
        OctTree<T>* child44 = child4->insert_local_child(1,0,0);
        OctTree<T>* child45 = child4->insert_local_child(1,0,1);
        OctTree<T>* child46 = child4->insert_local_child(1,1,0);
        OctTree<T>* child47 = child4->insert_local_child(1,1,1);

        OctTree<T>* child400 = child40->insert_local_child(0,0,0);
        OctTree<T>* child401 = child40->insert_local_child(0,0,1);
        OctTree<T>* child402 = child40->insert_local_child(0,1,0);
        OctTree<T>* child403 = child40->insert_local_child(0,1,1);
        OctTree<T>* child404 = child40->insert_local_child(1,0,0);
        OctTree<T>* child405 = child40->insert_local_child(1,0,1);
        OctTree<T>* child406 = child40->insert_local_child(1,1,0);
        OctTree<T>* child407 = child40->insert_local_child(1,1,1);

	child0->setData(3.14159);
	child1->setData(3.14159);
	child2->setData(3.14159);
	child3->setData(3.14159);
	child4->setData(3.14159);
	child5->setData(3.14159);
	child6->setData(3.14159);
	child7->setData(3.14159);

	child30->setData(3.14159);
	child31->setData(3.14159);
	child32->setData(3.14159);
	child33->setData(3.14159);
	child34->setData(3.14159);
	child35->setData(3.14159);
	child36->setData(3.14159);
	child37->setData(3.14159);

	child370->setData(3.14159);
	child371->setData(3.14159);
	child372->setData(3.14159);
	child373->setData(3.14159);
	child374->setData(3.14159);
	child375->setData(3.14159);
	child376->setData(3.14159);
	child377->setData(3.14159);

	child40->setData(3.14159);
	child41->setData(3.14159);
	child42->setData(3.14159);
	child43->setData(3.14159);
	child44->setData(3.14159);
	child45->setData(3.14159);
	child46->setData(3.14159);
	child47->setData(3.14159);

	child400->setData(3.14159);
	child401->setData(3.14159);
	child402->setData(3.14159);
	child403->setData(3.14159);
	child404->setData(3.14159);
	child405->setData(3.14159);
	child406->setData(3.14159);
	child407->setData(3.14159);

	/* Making the cost different */
        child4->setCost(2);
        child40->setCost(2);
        child3->setCost(2);
        child37->setCost(2);

	/********************************************************************************/
	/*										*/
	/* Now, partition the tree and send results to each processor			*/
	/*										*/
	/********************************************************************************/
	

	std::cout << "about to serialPartition: " << std::endl;
	t->serialPartition(nproc, &pieces);
	std::cout << "done with serialPartition" << std::endl;
	for (int i = 0; i < nproc; i++)
	{
	    int size = pieces[i].size();
	    for (int j = 0; j < size; j++)
	    {
		list.push_back(pieces[i][j]);
	    }
	}



/*
	list.push_back(RootList(t, 0, 3));
	list.push_back(RootList(child3, 0, 1));
	list.push_back(RootList(child2, 0, 0));
	list.push_back(RootList(child1, 0, 0));
	list.push_back(RootList(child0, 0, 0));
	list.push_back(RootList(child41, 0, 2));
	list.push_back(RootList(child40, 0, 2));
	list.push_back(RootList(child36, 0, 0));
	list.push_back(RootList(child35, 0, 0));
	list.push_back(RootList(child34, 0, 0));
	list.push_back(RootList(child33, 0, 0));
	list.push_back(RootList(child32, 0, 0));
	list.push_back(RootList(child31, 0, 0));
	list.push_back(RootList(child30, 0, 0));
	list.push_back(RootList(child370, 0, 0));
*/

	std::cout << "done with putting pieces on list" << std::endl;
	sort(list.begin(), list.end());
	std::cout << std::endl << "sorted list" << std::endl;
	int llength = list.size();
	for (int i = 0; i < llength; i++)
	{
	    std::cout << "Subtree: " << std::endl;
//	    list[i]->depthFirstTraverse();
	    std::cout << "Layer " << list[i].n << ": " <<
		"(" << list[i].x << "," << list[i].y << "," << list[i].z << ")" << std::endl;
	}

	std::vector<OctTree<T>* > *treeList = new std::vector<OctTree<T>* >();
	std::cout << "made treeList" << std::endl;
	treeList->push_back(t);
	std::cout << "added t to treeList" << std::endl;
	exchangeTrees(&list, treeList);

	// Let the processors know how many pieces they are getting

	std::cout << "received subtree; all done" << std::endl;
    }
    else // I am not processor 0
    {

	std::vector<RootList> *globalList = new std::vector<RootList>();
	std::vector<OctTree<T>* > *treeList = new std::vector<OctTree<T>* >();
	exchangeTrees(globalList, treeList);

/*
	for (int i = 0; i < 1001; i++)
	{
	   std::cout << " " << i;
	   std::cout.flush();
	}
*/
	std::cout << "End of the line " << std::endl;
    }

    MPI::Finalize();

 
    return 0;
}
    
