using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DijkstraAlgorithm
{
	class Program
	{
		static void Main(string[] args)
		{
			int nodes = 4;
			int[,] nodeArray = new int[nodes, nodes];
			int[] doneList = new int[nodes];
			int[,] dataArray = new int[,]
				{  // 0  1   2   3
					{0, 15, 1, 1 }, // 0
					{0, 0, 3,0 }, // 1
					{ 1,  3,0,0 }, // 2
					{0,  1, 1,0 }  // 3
				};

			for (int i = 0; i < nodes; i++)
			{
				ProcessArray(nodes, i, i, doneList, nodeArray, dataArray);
				doneList = new int[nodes];
			}
			printArray(nodeArray);
			Console.ReadLine();
		}

		static void ProcessArray(int nodes, int startNode, int current, int[] doneList, int[,] nodeArray, int[,] dataArray)
		{
			int val = 0;
			for(int i=0; i<nodes; i++)
			{
				if (i != startNode)
				{
					if(doneList[i] != 1)
					{
						if(dataArray[current,i] != 0)
						{
							val = nodeArray[startNode,current] + dataArray[current, i];
							if (nodeArray[startNode,i] == 0 || val < nodeArray[startNode,i])
							{
								nodeArray[startNode,i] = val;
							}
						}
					}
				}
			}

			doneList[current] = 1;
			int next = getLowestThatIsntDone(startNode,nodeArray, doneList);
			if(next != -1)
			{
				ProcessArray(nodes,startNode, next, doneList, nodeArray, dataArray);
			}
		}

		static void printArray(int[,] arr)
		{
			int lenA = arr.GetLength(0);
			int lenB = arr.GetLength(1);
			for(int i=0; i<lenA; i++)
			{
				for(int j = 0; j < lenB; j++)
				{
					Console.Write("{0} ", arr[i, j]);
				}
				Console.Write("\n");
			}
		}

		static int getLowestThatIsntDone(int startNode,int[,] nodeArray, int[] doneArray)
		{
			int lowest = -1;
			int node = -1;
			for(int i=0; i < nodeArray.GetLength(0); i++)
			{
				if(doneArray[i] != 1)
				{
					if (nodeArray[startNode, i] != 0)
					{
						if (lowest == -1 || nodeArray[startNode, i] < lowest)
						{
							lowest = nodeArray[startNode, i];
							node = i;
						}
					}
				}
			}
			return node;
		}
	}
}
