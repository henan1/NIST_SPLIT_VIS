#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "svException.h"
#include "svQDOTProcess.h"
#include "svConfig.h"

using namespace std;

#define DEFAULT_DELTA_T      0.001
#define DEFAULT_MIN_DELTA_T  0.001
#define DEFAULT_MAX_DELTA_T  0.08
#define DEFAULT_ERROR_TOLERANCE  0.0001

#define DEFAULT_MIN_BBOX  999
#define DEFAULT_MAX_BBOX  -999

namespace __svl_lib {

/*contour generation*/
svContour::svContour(){}//svVectorField *inputField)

void svContour::ComputeContourLines(char *vtkfName, char *contourfName, float contourValue){
      char *exe = new char[2048];
      sprintf(exe, "%s/Contour %s %s %g svl",  BIN_DIR,
              vtkfName, contourfName, contourValue);
      system(exe);
      delete [] exe;
}

void svContour::ComputeContours(char *vtkfName, char *contourfName, float contourValue){
 /*------------call the explicit prorams------------------------------*/
      char *exe = new char[2048];
      sprintf(exe, "%s/Contour %s %s %g",  BIN_DIR,
	      vtkfName, contourfName, contourValue);
      //cerr<<exe<<endl;		  
      system(exe); 
  delete [] exe;
}

void svContour::ProcessContour(char *contourfile, char *planename, char *dir, int layer){
     char *tmp = new char[200];
     sprintf(tmp, "%s/coor%s%d.txt", dir, planename, layer);

     svVector3 pos;
     svVector3 x;
     svVector3 y;
     svVector3 z;

     ifstream infile(tmp);
        infile>>pos[0]>>pos[1]>>pos[2];
        infile>>x[0]>>x[1]>>x[2];
        infile>>y[0]>>y[1]>>y[2];
        infile>>z[0]>>z[1]>>z[2];
     infile.close();

        delete [] tmp;
        infile.open(contourfile);
        int n;
        infile>>n;
        svVector3Array ppp;
        svVector3Array vvv;
        svScalarArray mmm;
        for(int i=0;i<n;i++)
        {
                double p[3]; svVector3 v; svScalar m;
                infile>>p[0]>>p[1]>>p[2]>>v[0]>>v[1]>>v[2]>>m;

                svVector3 pp;
                pp[0] = pos[0] + p[0] * x[0];
                pp[1] = pos[1] + p[0] * x[1];
                pp[2] = pos[2] + p[0] * x[2];
                pp[0] = pp[0] + p[1] * y[0];
                pp[1] = pp[1] + p[1] * y[1];
                pp[2] = pp[2] + p[1] * y[2];
                ppp.add(pp);
                vvv.add(v);
                mmm.add(m);
        }
        infile.close();

        ofstream outfile(contourfile);
        outfile<<n<<endl;
        for(int i=0;i<n;i++)
        {
                double p[7];
                outfile<<ppp[i][0]<<" "<<ppp[i][1]<<" "<<ppp[i][2]<<" "
                       <<vvv[i][0]<<" "<<vvv[i][1]<<" "<<vvv[i][2]<<" "
                       <<mmm[i]<<endl;
        }
        outfile.close() ;

        ppp.free();vvv.free();mmm.free();
}
/*contour end*/

/*KMeans clustering*/

svKmeans::svKmeans(){}
vector<Spin> svKmeans::Normalization(int klevel, double *mean, double *stdev, vector< vector<Spin> > data, KmeansProperty &property, vector<int> &index, vector<int> &slice){
		vector<Spin> clusterdata;
		for(int i=0;i<data.size();i++){
                        for(int l=0;l<data[i].size();l++){
			     Spin spin =  data[i][l];
                             if(property.roiP[klevel]==NULL){
                                        clusterdata.push_back(spin);
                                        slice.push_back(i);index.push_back(l);
                                        mean[0]+=spin.pos[0];
                                        mean[1]+=spin.pos[1];
                                        mean[2]+=spin.pos[2];
                                        mean[3]+=spin.dir[0];
                                        mean[4]+=spin.dir[1];
                                        mean[5]+=spin.dir[2];
                                        mean[6]+=spin.den;
                             } 
                             else if(isEqualLarger(spin.den,property.roiP[klevel]->minMag,true)
                             && isEqualSmaller(spin.den,property.roiP[klevel]->maxMag,true)
                             && isEqualLarger(spin.xangle,property.roiP[klevel]->minX,false)
                             && isEqualSmaller(spin.xangle,property.roiP[klevel]->maxX,false)
                             && isEqualLarger(spin.yangle,property.roiP[klevel]->minY,false)
                             && isEqualSmaller(spin.yangle,property.roiP[klevel]->maxY,false)
                             && isEqualLarger(spin.zangle,property.roiP[klevel]->minZ,false)
                             && isEqualSmaller(spin.zangle,property.roiP[klevel]->maxZ,false)
                                 && property.roiP[klevel]->splitVisible[i]){
                                        clusterdata.push_back(spin);
                                        slice.push_back(i);index.push_back(l);
                                        mean[0]+=spin.pos[0];
                                        mean[1]+=spin.pos[1];
                                        mean[2]+=spin.pos[2];
                                        mean[3]+=spin.dir[0];
                                        mean[4]+=spin.dir[1];
                                        mean[5]+=spin.dir[2];
                                        mean[6]+=spin.den;
                                }
                                /*else{
cerr<<klevel<<" "<<property.roiP[klevel]->minMag<<" "<<property.roiP[klevel]->maxMag<<" "<<property.roiP[klevel]->maxX<<" "<<property.roiP[klevel]->minX<<" "<<property.roiP[klevel]->maxY<<" "<<property.roiP[klevel]->minY<<" "<<property.roiP[klevel]->maxZ<<" "<<property.roiP[klevel]->minZ<<" "<<property.roiP[klevel]->splitVisible[i]<<endl;
                                   cout<<spin.pos[0]<<" "<<spin.pos[1]<<" "<<spin.pos[2]<<" "
                                       <<spin.xangle<<" "<<spin.yangle<<" "<<spin.zangle<<" "
                                       <<spin.den<<endl;}*/
                        }
                }
                for(int l=0;l<property.dimension[klevel];l++)
                         if(clusterdata.size()>0)mean[l]=mean[l]/clusterdata.size();
                for(int l=0;l<clusterdata.size();l++){
                                stdev[0]+=pow(clusterdata[l].pos[0]-mean[0],2);
                                stdev[1]+=pow(clusterdata[l].pos[1]-mean[1],2);
                                stdev[2]+=pow(clusterdata[l].pos[2]-mean[2],2);
                                stdev[3]+=pow(clusterdata[l].dir[0]-mean[3],2);
                                stdev[4]+=pow(clusterdata[l].dir[1]-mean[4],2);
                                stdev[5]+=pow(clusterdata[l].dir[2]-mean[5],2);
                                stdev[6]+=pow(clusterdata[l].den-mean[6],2);
                 }
                for(int l=0;l<property.dimension[klevel];l++){
                        if(clusterdata.size()>0) stdev[l]= sqrt(stdev[l]/clusterdata.size());
                        else stdev[l]=1;//de>0?de:1;
                        stdev[l]=stdev[l]>0?stdev[l]:1;
                }
        for(int l=0;l<clusterdata.size();l++){
                clusterdata[l].pos[0]=(clusterdata[l].pos[0]- mean[0])/stdev[0];
                clusterdata[l].pos[1]=(clusterdata[l].pos[1]- mean[1])/stdev[1];
                clusterdata[l].pos[2]=(clusterdata[l].pos[2]- mean[2])/stdev[2];
                clusterdata[l].dir[0]=(clusterdata[l].dir[0]- mean[3])/stdev[3];
                clusterdata[l].dir[1]=(clusterdata[l].dir[1]- mean[4])/stdev[4];
                clusterdata[l].dir[2]=(clusterdata[l].dir[2]- mean[5])/stdev[5];
                clusterdata[l].den=(clusterdata[l].den- mean[6])/stdev[6];
                clusterdata[l].pos[0]*=property.weights[klevel][0];
                clusterdata[l].pos[1]*=property.weights[klevel][1];
                clusterdata[l].pos[2]*=property.weights[klevel][2];
                clusterdata[l].dir[0]*=property.weights[klevel][3];
                clusterdata[l].dir[1]*=property.weights[klevel][4];
                clusterdata[l].dir[2]*=property.weights[klevel][5];
                clusterdata[l].den*=property.weights[klevel][6];
        }
	return clusterdata;
}
vector<Spin> svKmeans::Normalization(int klevel, double *mean, double *stdev, vector<Spin> data, KmeansProperty &property, vector<int> &index, vector<int> &slice, int sliceindex){
	vector<Spin> clusterdata;
        for(int l=0;l<data.size();l++){
		Spin spin =  data[l];
                if(property.roiP[klevel]==NULL){
                        clusterdata.push_back(spin);
                        index.push_back(l);
                        slice.push_back(sliceindex);
                        mean[0]+=spin.pos[0];
                        mean[1]+=spin.pos[1];
                        mean[2]+=spin.pos[2];
                        mean[3]+=spin.dir[0];
                        mean[4]+=spin.dir[1];
                        mean[5]+=spin.dir[2];
                        mean[6]+=spin.den;
                }
                else if(isEqualLarger(spin.den,property.roiP[klevel]->minMag,true)
                   && isEqualSmaller(spin.den,property.roiP[klevel]->maxMag,true)
                   && isEqualLarger(spin.xangle,property.roiP[klevel]->minX,false)
                   && isEqualSmaller(spin.xangle,property.roiP[klevel]->maxX,false)
                   && isEqualLarger(spin.yangle,property.roiP[klevel]->minY,false)
                   && isEqualSmaller(spin.yangle,property.roiP[klevel]->maxY,false)
                   && isEqualLarger(spin.zangle,property.roiP[klevel]->minZ,false)
                   && isEqualSmaller(spin.zangle,property.roiP[klevel]->maxZ,false)
                   && property.roiP[klevel]->splitVisible[sliceindex]){//TODO
                        clusterdata.push_back(spin);
 			index.push_back(l);
			slice.push_back(sliceindex);
                        mean[0]+=spin.pos[0];
                        mean[1]+=spin.pos[1];
                        mean[2]+=spin.pos[2];
                        mean[3]+=spin.dir[0];
                        mean[4]+=spin.dir[1];
                        mean[5]+=spin.dir[2];
                        mean[6]+=spin.den;
                   }
        }//cerr<<clusterdata.size()<<endl;
        for(int l=0;l<property.dimension[klevel];l++)
               if(clusterdata.size()>0)mean[l]=mean[l]/clusterdata.size();
        for(int l=0;l<clusterdata.size();l++){
              stdev[0]+=pow(clusterdata[l].pos[0]-mean[0],2);
              stdev[1]+=pow(clusterdata[l].pos[1]-mean[1],2);
              stdev[2]+=pow(clusterdata[l].pos[2]-mean[2],2);
              stdev[3]+=pow(clusterdata[l].dir[0]-mean[3],2);
              stdev[4]+=pow(clusterdata[l].dir[1]-mean[4],2);
              stdev[5]+=pow(clusterdata[l].dir[2]-mean[5],2);
              stdev[6]+=pow(clusterdata[l].den-mean[6],2);
        }
        for(int l=0;l<property.dimension[klevel];l++){
              if(clusterdata.size()>0)  stdev[l] =sqrt(stdev[l]/clusterdata.size());
              else stdev[l] = 1.;
              stdev[l]=stdev[l]>0?stdev[l]:1;
        }
        for(int l=0;l<clusterdata.size();l++){
              clusterdata[l].pos[0]=(clusterdata[l].pos[0]- mean[0])/stdev[0];
              clusterdata[l].pos[1]=(clusterdata[l].pos[1]- mean[1])/stdev[1];
              clusterdata[l].pos[2]=(clusterdata[l].pos[2]- mean[2])/stdev[2];
              clusterdata[l].dir[0]=(clusterdata[l].dir[0]- mean[3])/stdev[3];
              clusterdata[l].dir[1]=(clusterdata[l].dir[1]- mean[4])/stdev[4];
              clusterdata[l].dir[2]=(clusterdata[l].dir[2]- mean[5])/stdev[5];
              clusterdata[l].den=(clusterdata[l].den- mean[6])/stdev[6];
              clusterdata[l].pos[0]*=property.weights[klevel][0];
              clusterdata[l].pos[1]*=property.weights[klevel][1];
              clusterdata[l].pos[2]*=property.weights[klevel][2];
              clusterdata[l].dir[0]*=property.weights[klevel][3];
              clusterdata[l].dir[1]*=property.weights[klevel][4];
              clusterdata[l].dir[2]*=property.weights[klevel][5];
              clusterdata[l].den*=property.weights[klevel][6];
        }
	return clusterdata;
}
void svKmeans::SavetoNormalFile(vector<Spin> data, char *normalizefName){
  ofstream outfile(normalizefName);
  //cerr<<data.size()<<" "<<normalizefName<<endl;
  for(int i=0;i<data.size();i++){
    outfile<<data[i].pos[0]<<" "<<data[i].pos[1]<<" "<<data[i].pos[2]<<" "
           <<data[i].dir[0]<<" "<<data[i].dir[1]<<" "<<data[i].dir[2]<<" "
	   <<data[i].den<<endl;
  }
  outfile.close();
}
void svKmeans::SetClusterLabel(char *clusterfName, int **clusterList,
				vector<int> index1, vector<int> index2, int level,
                                int &maxCluster){
//  cerr<<maxCluster<<" "<<level<<endl;
  ifstream infile(clusterfName);
  int c;
  int count = 0;
  while(infile>>c){
    int i = index1[count];
    int j = index2[count];
    if(clusterList[i][j]==-1) clusterList[i][j] = c;
    else{
       clusterList[i][j]=level +c ;
    }
    if(clusterList[i][j] > maxCluster) maxCluster = clusterList[i][j];
    count++;
  }
  infile.close();
}
//assuming that all levels of clustering share the same "isSplit"
void svKmeans::SortClusterLabel(vector< vector<Spin> > data, bool isSplit, int **clusterList, int &maxCluster){
  if(isSplit){
    int newMaxCluster=-1;
    for(int i=0;i<data.size();i++){
      //cerr<<i<<" "<<newMaxCluster<<" "<<maxCluster<<endl;
      double *sum = new double[maxCluster+1];
      int *count = new int[maxCluster+1];
      for(int j=0;j<maxCluster+1;j++){sum[j]=0;count[j]=0;}
      for(int j=0;j<data[i].size();j++){
        int c = clusterList[i][j];
        if(c>=0){
          sum[c] += data[i][j].den; 
          count[c]++;
        }
      }//cerr<<data[i].size()<<endl;
      double *tmpsum = new double[maxCluster+1];
      for(int j=0;j<maxCluster+1;j++){
        if(count[j]>0)sum[j]=sum[j]/(double)count[j];
        tmpsum[j] = sum[j];//cerr<<count[j]<<endl;
      }
      sort(tmpsum,tmpsum+maxCluster+1, sortScalarLargetoSmall);
      int tmpMaxCluster = -1;
      for(int j=0;j<data[i].size();j++){
        int c= clusterList[i][j];
        if(c<0) continue;
        int index = 0;
        for(int s=0;s<maxCluster+1;s++){
            if(isEqual(tmpsum[s],sum[c],true)){
                 if(s>tmpMaxCluster)tmpMaxCluster=s;
                 index = s;break;
            }
        }
        clusterList[i][j] = index;
      }
      ///cerr<<tmpMaxCluster<<endl;
      //if(tmpMaxCluster > newMaxCluster) newMaxCluster = tmpMaxCluster;
      newMaxCluster += tmpMaxCluster+1;
      delete [] sum; delete [] count; delete [] tmpsum;
    }
    maxCluster = newMaxCluster + 1 + 1;
  }
  else{
    //cout<<maxCluster<<endl;
    double *sum = new double[maxCluster+1];
    int *count = new int[maxCluster+1];
    for(int j=0;j<maxCluster+1;j++){sum[j]=0;count[j]=0;}
    for(int i=0;i<data.size();i++){
      for(int j=0;j<data[i].size();j++){
         int c=clusterList[i][j];
         if(c>=0){
          sum[c] += data[i][j].den;
          count[c]++;
         }
      } 
    }
    double *tmpsum = new double[maxCluster+1];
    for(int j=0;j<maxCluster+1;j++){
      if(count[j]>0)sum[j]=sum[j]/(double)count[j];
      tmpsum[j] = sum[j];
    }
    sort(tmpsum,tmpsum+maxCluster+1, sortScalarLargetoSmall);
    int tmpMaxCluster = -1;
    for(int i=0;i<data.size();i++){
      for(int j=0;j<data[i].size();j++){
        int c = clusterList[i][j];
        if(c < 0) continue;
        int index = 0;
        for(int s=0;s<maxCluster+1;s++){
            if(isEqual(tmpsum[s], sum[c], true)){
             if(s>tmpMaxCluster)tmpMaxCluster=s;
             index = s;
             break;
            }
        }
        clusterList[i][j] = index;
      }
    }
    maxCluster = tmpMaxCluster+1+1;//<----the number of clusters
    //cerr<<maxCluster<<endl;
    //for(int i=0;i<=tmpMaxCluster;i++)cerr<<tmpsum[i]<<endl;
    delete [] sum; delete [] count; delete [] tmpsum;
  }  
}
int svKmeans::ComputeClusters(char *datafName, char *normalizefName, char *clusterfName,
				KmeansProperty &property,
                                int ** clusterList){//TODO: make the kmeans algorithm stable
   vector< vector<Spin> > data;
   int maxCluster=-1;
   ifstream infile(datafName);
   int n; infile>>n; data.resize(n);
   for(int i=0;i<n;i++){
	int m;infile>>m;
        for(int j=0;j<m;j++){
		Spin tmp;
		infile>>tmp.pos[0]>>tmp.pos[1]>>tmp.pos[2]
			>>tmp.dir[0]>>tmp.dir[1]>>tmp.dir[2]
			>>tmp.den;
                tmp.xangle = acos(tmp.dir[0])*180./PI;
                tmp.yangle = acos(tmp.dir[1])*180./PI;
                tmp.zangle = acos(tmp.dir[2])*180./PI;
		data[i].push_back(tmp);		
	}
       //cerr<<data[i].size()<<endl;
   }
   infile.close();
   int level = 0;
   for(int i=0;i<property.numCluster.size();i++){
	if(property.isSplit[i] == true){//<--FIX
		for(int j=0;j<data.size();j++){
			double mean[7]={0,0,0,0,0,0,0};
			double stdev[7]={0,0,0,0,0,0,0};
                        vector<int> index;
                        vector<int> slice;
			vector<Spin> clusterdata=Normalization(i, mean,stdev,data[j],property,
                                                               index,slice,j);
                        if(clusterdata.size()>0){
                          SavetoNormalFile(clusterdata, normalizefName);//<--check
                          char *exe = new char[2048];
                          sprintf(exe,"%s/kmlsample -d %d -k %d -max %d -df %s > %s 2>&1",  
                                BIN_DIR, property.dimension[i], property.numCluster[i],
                                clusterdata.size(), normalizefName, clusterfName);
                          system(exe);//cerr<<exe<<endl;
                          SetClusterLabel(clusterfName, clusterList,slice, index,level,maxCluster); 
			  clusterdata.clear(); 
		       	  delete [] exe; 
                        }
                        index.clear(); slice.clear();
		}	
        }
        else{
		double mean[7]={0,0,0,0,0,0,0};
		double stdev[7]={0,0,0,0,0,0,0};
		vector<int>slice;vector<int>index;//cerr<<mean[0]<<endl;
		vector<Spin> clusterdata=Normalization(i, mean,stdev,data,property,index,slice);
		SavetoNormalFile(clusterdata, normalizefName);
		char *exe = new char[2048];
		sprintf(exe,"%s/kmlsample -d %d -k %d -max %d -df %s > %s 2>&1",
		          BIN_DIR, property.dimension[i], property.numCluster[i],
		          clusterdata.size(), normalizefName, clusterfName);
		system(exe);
                cerr<<exe<<endl;
		SetClusterLabel(clusterfName, clusterList, slice,index,level,maxCluster);
		clusterdata.clear();
		delete [] exe; slice.clear(); index.clear();
	} 
        level += property.numCluster[i];
   } 
   SortClusterLabel(data, property.isSplit[0], clusterList, maxCluster); 
   for(int i=0;i<data.size();i++) data[i].clear();
   data.clear();
   cerr<<"cluster done"<<endl;

   return maxCluster;
}

void svKmeans::Normalization(char *datafName, char *normalizefName, 
				int dim, int dataSize, svScalarArray weight){
	vector< vector<double> >  data;
	vector<double> mean(dim);
	vector<double> stdev(dim);
	for(int i=0;i<dim;i++)
	{
		mean[i] = 0;	
		stdev[i] = 0;
	}

	ifstream infile(datafName);
	ofstream outfile(normalizefName);
		
	for(int i=0;i<dataSize;i++)
	{
		vector<double> temp_data(dim);
		
		for(int j=0;j<dim;j++)
		{
			infile>>temp_data[j];
			mean[j] = mean[j] + temp_data[j];
		}
		data.push_back(temp_data);
		
		temp_data.clear();
	}
	
	infile.close();
	

	if(data.size()>0)
	{
		for(int i=0;i<dim;i++)
		{
			mean[i] = mean[i]/data.size();
		}		
		for(int i=0;i<data.size();i++)
		{
			for(int j=0;j<dim;j++)
			{
				stdev[j] = stdev[j] + (data[i][j]-mean[j])*(data[i][j]-mean[j]);
			}
		}
		for(int i=0;i<dim;i++)
		{
			stdev[i] = sqrt(stdev[i]/(data.size()-1));
		//	cerr<<mean[i]<<" "<<stdev[i]<<" "<<data.size()<<endl;
			if(stdev[i]==0) stdev[i] = 1;
		}
	}
	
	for(int i=0;i<data.size();i++)
	{
		for(int j=0;j<dim;j++)
		{
	//		if(j==dim-1)
	//		cerr<<mean[j]<<" "<<stdev[j]<<" "<<data[i][j]<<" "<<(data[i][j] - mean[j])/stdev[j] * weight[j]<<endl;
			outfile<<(data[i][j] - mean[j])/stdev[j] * weight[j]<<" ";
		}
		outfile<<endl;
	}		
	outfile.close();

	for(int i=0;i<data.size();i++)
	{
		data[i].clear();
	}
	data.clear();
	mean.clear();
	stdev.clear();
}


void svKmeans::ComputeClusters2(char *datafName, char *clusterfName, 
								int dim, int dataSize, int numCluster){
			char *exe = new char[2048];

			sprintf(exe, "%s/kmlsample -d %d -k %d -max %d -df %s > %s 2>&1",  BIN_DIR,
			dim,
			numCluster,
			dataSize,
			datafName,
			clusterfName);
			
			system(exe);

			delete [] exe;
}

/*KMeans end*/

/*Neighbors*/
svNeighbor::svNeighbor()//svVectorField *inputfield)
{
}

void svNeighbor::ComputeNeighbors(NeighborProperty &property)
{
   cout<<"Neighbor processing ..."<<endl;

  ifstream test(property.outputfile);
//cerr<<property.outputdir<<" "<<str<<endl;
  if(!test.is_open())
  {
   svVector3Array *pos;
   svIntArray *label;
   svIntArray *index;

   ifstream infile(property.inputfile);
   int n;
   infile>>n;
   pos = new svVector3Array[n];
   index=  new svIntArray[n];
   int N = 0;
   for(int i=0;i<n;i++)
   {
       int m;
       infile>>m;
       for(int j=0;j<m;j++)
       {
         svVector3 p,d;
         svScalar m;
         infile>>p[0]>>p[1]>>p[2]
             >>d[0]>>d[1]>>d[2]>>m;
         pos[i].add(p);
         index[i].add(N);
         N++;
       }
   }
   label = new svIntArray[N];

//cerr<<"========================================================="<<endl;
   infile.close();
  
   int size = property.svectors.size();
   int count = 0;

   for(int i=0;i<n;i++)
   {
 //     int percentage = i*100/n;
//      ProgressBar(percentage);

      for(int j=0;j<pos[i].size();j++)
      {
        vector<svVector3> pair;
        pair.resize(size);
        vector<int> planeindex;
        planeindex.resize(size);

        for(int t=0;t<size;t++)
        {
            pair[t] = pos[i][j] + property.svectors[t];
           
            svScalar distance = dot(pair[t] - property.planepos, 
                                    property.planedir);
            planeindex[t] = distance/property.planedistance;

            int layer = planeindex[t];
            
            if(layer >=0 && layer<n)
            {
                 for(int tt=0;tt<pos[layer].size();tt++)
                 {
                      if(fabs(pos[layer][tt][0] - pair[t][0])<1e-3
                      && fabs(pos[layer][tt][1] - pair[t][1])<1e-3
                      && fabs(pos[layer][tt][2] - pair[t][2])<1e-3
                      &&count>index[layer][tt])
                   {
                       label[count].add(index[layer][tt]);break;
                   }
                 }
            }
        }
        
        int ii;
        svScalar zz;
        zz = pos[i][j][2] - (int)pos[i][j][2];
        if(zz<0) zz = zz + 1.;

        for(int t=0;t<property.zvalues.size();t++)
        {
              if(fabs(zz - property.zvalues[t])<1e-3) 
              {
                 ii = t;break;
              } 
        }  
        size = property.dvectors[ii].size();

        pair.resize(size);
        planeindex.resize(size);

        for(int t=0;t<size;t++)
        {
            pair[t] = pos[i][j] + property.dvectors[ii][t];
            svScalar distance = dot(pair[t] - property.planepos,
                                    property.planedir);
            planeindex[t] = distance/property.planedistance;

            int layer = planeindex[t];

            if(layer >=0 && layer<n)
            {
                 for(int tt=0;tt<pos[layer].size();tt++)
                 {
                      if(fabs(pos[layer][tt][0] - pair[t][0])<1e-3
                      && fabs(pos[layer][tt][1] - pair[t][1])<1e-3
                      && fabs(pos[layer][tt][2] - pair[t][2])<1e-3
                     && count>index[layer][tt])
                   {
                       label[count].add(index[layer][tt]);break;
                   }
                 }
            }
        }

        pair.clear();
        planeindex.clear();
        count++;
      }
   }
//-----------savetofile-----------------

   ofstream outfile(property.outputfile);
   outfile<<N<<endl;
   for(int i=0;i<N;i++)
   {
      outfile<<label[i].size()<<" ";
      for(int j=0;j<label[i].size();j++)
      {
          outfile<<label[i][j]<<" ";
      } outfile<<endl;
   }
   outfile<<endl;

   outfile.close();

   for(int i=0;i<n;i++)
   {
       pos[i].free();
       index[i].free();
   }
   delete [] pos;
   delete [] index;
   for(int i=0;i<N;i++)
          label[i].free();
   delete [] label;
  }
  else 
    test.close();   
}
/*=============Neighbors End==================================*/

/*Near*/
svNear::svNear(){
}
void svNear::ComputeComponents(vector<Spin *> data, 
                             int &numOfComponents,
                             vector<int> &connectIndex,
                             svScalar minD){
 int nPts;
 ANNpointArray   dataPts;                // data points
 ANNpoint        queryPt;                // query point
 ANNidxArray     nnIdx;                  // near neighbor indices
 ANNdistArray    dists;                  // near neighbor indices
 int num = 20;
 nnIdx = new ANNidx[num];                  // allocate near neigh indices
 dists = new ANNdist[num];                 // allocate near neighbor dists
 int dim = 3;
 queryPt = annAllocPt(dim);

//=====================intialization====================//
 map<Spin *, int> pos;
 numOfComponents = 1;
 for(int i=0;i<data.size();i++){
   pos.insert(std::pair<Spin*, int>(data[i],i));
   connectIndex.push_back(-1);
 }
 nPts = data.size();
 dataPts= annAllocPts(nPts*dim, dim);
 for(int j=0;j<data.size();j++){
       dataPts[j][0] = (*data[j]).pos[0];
       dataPts[j][1] = (*data[j]).pos[1];
       dataPts[j][2] = (*data[j]).pos[2];
 }
 cerr<<data.size()<<endl;
 ANNkd_tree *kdTree = new ANNkd_tree(dataPts,nPts, dim);//kd tree
//===================================================//
 int index = 0; //<-label. index+1 is equal to num of connected components
 while(!pos.empty()){
    map<Spin*, int > tmppos;
    tmppos.insert(std::pair<Spin*, int>(pos.begin()->first,pos.begin()->second));
    pos.erase(pos.begin()->first);
    connectIndex[tmppos.begin()->second]=index;
    
    int count=0;
    while(!tmppos.empty()){
          Spin spin = *(tmppos.begin()->first);
          queryPt[0] = spin.pos[0];
          queryPt[1] = spin.pos[1];
          queryPt[2] = spin.pos[2];
          kdTree->annkSearch(queryPt, num, nnIdx, dists, 0.0);
          for(int i=1;i<num;i++){
            if(dists[i]<minD && connectIndex[nnIdx[i]]<0){
              connectIndex[nnIdx[i]] = index; count++;
              tmppos.insert(std::pair<Spin*, int>(data[nnIdx[i]], nnIdx[i]));
              pos.erase(data[nnIdx[i]]);
            }
          }
          tmppos.erase(tmppos.begin()->first);
    }
//cerr<<count<<endl;
    index++;
    numOfComponents++;
 }
}
void svNear::ComputeNear(svVector3Array *pos1, svVector3Array *pos2,
                         int seed, svInt *index){
       int     nPts;                           // actual number of data points
       ANNpointArray   dataPts;                // data points
       ANNpoint        queryPt;                // query point
       ANNidxArray     nnIdx;                  // near neighbor indices
       ANNdistArray    dists;                  // near neighbor distances
//       ANNkd_tree*     kdTree;                 // search structure
       nnIdx = new ANNidx[1];                  // allocate near neigh indices
       dists = new ANNdist[1];                 // allocate near neighbor dists
       int dim = 3;
       queryPt = annAllocPt(dim);                         // allocate query point

       int count = 0;
       for(int i=0;i<seed;i++){
        count = 0;
        nPts = pos1[i].size();
        dataPts = annAllocPts(nPts*dim, dim);
        for(int j=0;j<pos1[i].size();j++){
           dataPts[j][0] = pos1[i][j][0];
           dataPts[j][1] = pos1[i][j][1];
           dataPts[j][2] = pos1[i][j][2];
        }
        ANNkd_tree *kdTree = new ANNkd_tree(dataPts,               // the data points
                                        nPts,                  // number of points
                                        dim);                   // dimension of space 
        for(int j=0;j<pos2[i].size();j++){
          queryPt[0] = pos2[i][j][0];
          queryPt[1] = pos2[i][j][1];
          queryPt[2] = pos2[i][j][2];

          kdTree->annkSearch(                                               // search
                                queryPt,                               // query point
                                1,                     // number of near neighbors
                                nnIdx,                // nearest neighbors (returned)
                                dists,                 // distance (returned)
                                0.0);           // error bound
          index[count] = nnIdx[0];
          count++;
        }
        delete kdTree;
        dataPts = annAllocPts(0,0);
       }
    delete [] nnIdx;
    delete [] dists;
    queryPt = annAllocPt(0);
    annClose();                     // done with ANN
}
void svNear::ComputeNear(svVector3Array *pos1, int seed1,
                       svVector3Array pos2, int region,
                       svIntArray &layer, svIntArray &index){
       int     nPts;                         // actual number of data points
       ANNpointArray   dataPts;              // data points
       ANNpoint        queryPt;              // query point
       ANNidxArray     nnIdx;                // near neighbor indices
       ANNdistArray    dists;                // near neighbor distances
       nnIdx = new ANNidx[1];                // allocate near neigh indices
       dists = new ANNdist[1];               // allocate near neighbor dists
       int dim = 3;
       queryPt = annAllocPt(dim);                         // allocate query point
//================================================================================
       nPts = 0;
       for(int i=0;i<seed1;i++){
        for(int j=0;j<pos1[i].size();j++){
         nPts++;
        }
       }
       vector<int> in[2];
       dataPts = annAllocPts(nPts*dim, dim);
       int count = 0;
       for(int i=0;i<seed1;i++){
        for(int j=0;j<pos1[i].size();j++){
           dataPts[count][0] = pos1[i][j][0];
           dataPts[count][1] = pos1[i][j][1];
           dataPts[count][2] = pos1[i][j][2];
           in[0].push_back(i);
           in[1].push_back(j);
           count++;
        }
       }
       ANNkd_tree *kdTree = new ANNkd_tree(dataPts,               // the data points
                                        nPts,                  // number of points
                                        dim);                   // dimension of space 
//cerr<<"ann.........."<<endl;
//=====================================================================
       for(int i=0;i<pos2.size();i++){
          queryPt[0] = pos2[i][0];
          queryPt[1] = pos2[i][1];
          queryPt[2] = pos2[i][2];
          kdTree->annkSearch(                                               // search
                                queryPt,                               // query point
                                1,                     // number of near neighbors
                                nnIdx,                // nearest neighbors (returned)
                                dists,                 // distance (returned)
                                0.0);           // error bound
          layer[i] = in[0][nnIdx[0]];
          index[i] = in[1][nnIdx[0]];
          //cerr<<pos1[layer[i]][index[i]][0]<<" "
//<<pos1[layer[i]][index[i]][1]<<" "
//<<pos1[layer[i]][index[i]][2]<<endl;
      }
//====================================================================
      delete kdTree;
      in[0].clear();
      in[1].clear();
      dataPts = annAllocPts(0,0);
      delete [] nnIdx;
      delete [] dists;
      queryPt = annAllocPt(0);
      annClose();                     // done with ANN
}
void svNear::ComputeNear(svVector3Array pos1, 
                         svVector3Array pos2, 
                         svIntArray &index){
       int     nPts;                         // actual number of data points
       ANNpointArray   dataPts;              // data points
       ANNpoint        queryPt;              // query point
       ANNidxArray     nnIdx;                // near neighbor indices
       ANNdistArray    dists;                // near neighbor distances
       nnIdx = new ANNidx[1];                // allocate near neigh indices
       dists = new ANNdist[1];               // allocate near neighbor dists
       int dim = 3;
       queryPt = annAllocPt(dim);                         // allocate query point
//================================================================================
       nPts = pos1.size();
       dataPts = annAllocPts(nPts*dim, dim);
       int count = 0;
       for(int j=0;j<pos1.size();j++){
           dataPts[count][0] = pos1[j][0];
           dataPts[count][1] = pos1[j][1];
           dataPts[count][2] = pos1[j][2];
           count++;
       }
       ANNkd_tree *kdTree = new ANNkd_tree(dataPts,               // the data points
                                        nPts,                  // number of points
                                        dim);                   // dimension of space 
//cerr<<"ann.........."<<endl;
//=====================================================================
       for(int i=0;i<pos2.size();i++){
          queryPt[0] = pos2[i][0];
          queryPt[1] = pos2[i][1];
          queryPt[2] = pos2[i][2];
          kdTree->annkSearch(                                               // search
                                queryPt,                               // query point
                                1,                     // number of near neighbors
                                nnIdx,                // nearest neighbors (returned)
                                dists,                 // distance (returned)
                                0.0);           // error bound
          index.add( nnIdx[0]);
          //cerr<<pos1[layer[i]][index[i]][0]<<" "
//<<pos1[layer[i]][index[i]][1]<<" "
//<<pos1[layer[i]][index[i]][2]<<endl;
      }
//====================================================================
      delete kdTree;
      dataPts = annAllocPts(0,0);
      delete [] nnIdx;
      delete [] dists;
      queryPt = annAllocPt(0);
      annClose();                     // done with ANN
}
void svNear::ComputeNear(vector<svVector3> pos1, 
                         vector<svVector3> pos2, 
                         svInt *index){
       int     nPts;                         // actual number of data points
       ANNpointArray   dataPts;              // data points
       ANNpoint        queryPt;              // query point
       ANNidxArray     nnIdx;                // near neighbor indices
       ANNdistArray    dists;                // near neighbor distances
       nnIdx = new ANNidx[1];                // allocate near neigh indices
       dists = new ANNdist[1];               // allocate near neighbor dists
       int dim = 3;
       queryPt = annAllocPt(dim);                         // allocate query point
//================================================================================
       nPts = pos1.size();
       dataPts = annAllocPts(nPts*dim, dim);
       int count = 0;
       for(int j=0;j<pos1.size();j++){
           dataPts[count][0] = pos1[j][0];
           dataPts[count][1] = pos1[j][1];
           dataPts[count][2] = pos1[j][2];
           count++;
       }
       ANNkd_tree *kdTree = new ANNkd_tree(dataPts,               // the data points
                                        nPts,                  // number of points
                                        dim);                   // dimension of space 
//cerr<<"ann.........."<<endl;
//=====================================================================
       for(int i=0;i<pos2.size();i++){
          queryPt[0] = pos2[i][0];
          queryPt[1] = pos2[i][1];
          queryPt[2] = pos2[i][2];
          kdTree->annkSearch(                                               // search
                                queryPt,                               // query point
                                1,                     // number of near neighbors
                                nnIdx,                // nearest neighbors (returned)
                                dists,                 // distance (returned)
                                0.0);           // error bound
          index[i] = nnIdx[0];
          //cerr<<pos1[layer[i]][index[i]][0]<<" "
//<<pos1[layer[i]][index[i]][1]<<" "
//<<pos1[layer[i]][index[i]][2]<<endl;
      }
//====================================================================
      delete kdTree;
      dataPts = annAllocPts(0,0);
      delete [] nnIdx;
      delete [] dists;
      queryPt = annAllocPt(0);
      annClose();                     // done with ANN
}
void svNear::ComputeNear(vector<svVector3> pos1, 
                         vector<svVector3> pos2, 
                         vector<svVector3> norm,
                         svInt *index){
       int     nPts;                         // actual number of data points
       ANNpointArray   dataPts;              // data points
       ANNpoint        queryPt;              // query point
       ANNidxArray     nnIdx;                // near neighbor indices
       ANNdistArray    dists;                // near neighbor distances
       int size = 10;
       nnIdx = new ANNidx[size];                // allocate near neigh indices
       dists = new ANNdist[size];               // allocate near neighbor dists
       int dim = 3;
       queryPt = annAllocPt(dim);                         // allocate query point
//================================================================================
       nPts = pos1.size();
       dataPts = annAllocPts(nPts*dim, dim);
       int count = 0;
       for(int j=0;j<pos1.size();j++){
           dataPts[count][0] = pos1[j][0];
           dataPts[count][1] = pos1[j][1];
           dataPts[count][2] = pos1[j][2];
           count++;
       }
       ANNkd_tree *kdTree = new ANNkd_tree(dataPts,               // the data points
                                        nPts,                  // number of points
                                        dim);                   // dimension of space 
//cerr<<"ann.........."<<endl;
//=====================================================================
       for(int i=0;i<pos2.size();i++){
          queryPt[0] = pos2[i][0];
          queryPt[1] = pos2[i][1];
          queryPt[2] = pos2[i][2];
          kdTree->annkSearch(                                               // search
                                queryPt,                               // query point
                                size,                     // number of near neighbors
                                nnIdx,                // nearest neighbors (returned)
                                dists,                 // distance (returned)
                                0.0);           // error bound
          double dd = 9e+9; double td;
          int in=0;
          for(int jj=0;jj<size;jj++){
            td =  PointtoLine2(pos2[i], norm[i], pos1[nnIdx[jj]]);
            if(td<dd){dd = td; in = nnIdx[jj];}
          }
          index[i] = in;
          //cerr<<pos1[layer[i]][index[i]][0]<<" "
//<<pos1[layer[i]][index[i]][1]<<" "
//<<pos1[layer[i]][index[i]][2]<<endl;
      }
//====================================================================
      delete kdTree;
      dataPts = annAllocPts(0,0);
      delete [] nnIdx;
      delete [] dists;
      queryPt = annAllocPt(0);
      annClose();                     // done with ANN
}
void svNear::ComputeNear(svVector3Array pos1, 
                         svVector3Array pos2, 
                         svIntArray *index){
       int     nPts;                         // actual number of data points
       ANNpointArray   dataPts;              // data points
       ANNpoint        queryPt;              // query point
       ANNidxArray     nnIdx;                // near neighbor indices
       ANNdistArray    dists;                // near neighbor distances
       nnIdx = new ANNidx[10];                // allocate near neigh indices
       dists = new ANNdist[10];               // allocate near neighbor dists
       int dim = 3;
       queryPt = annAllocPt(dim);                         // allocate query point
//================================================================================
       nPts = pos1.size();
       dataPts = annAllocPts(nPts*dim, dim);
       int count = 0;
       for(int j=0;j<pos1.size();j++){
           dataPts[count][0] = pos1[j][0];
           dataPts[count][1] = pos1[j][1];
           dataPts[count][2] = pos1[j][2];
           count++;
       }
       ANNkd_tree *kdTree = new ANNkd_tree(dataPts,               // the data points
                                        nPts,                  // number of points
                                        dim);                   // dimension of space 
//cerr<<"ann.........."<<endl
       index = new svIntArray[pos2.size()];
//=====================================================================
       for(int i=0;i<pos2.size();i++){
          queryPt[0] = pos2[i][0];
          queryPt[1] = pos2[i][1];
          queryPt[2] = pos2[i][2];
          kdTree->annkSearch(                                               // search
                                queryPt,                               // query point
                                10,                     // number of near neighbors
                                nnIdx,                // nearest neighbors (returned)
                                dists,                 // distance (returned)
                                0.0);           // error bound
          index[i].add( nnIdx[0]);
          for(int j=1;j<10;j++){
            if(fabs(dists[j]-dists[j-1])<1e-8){
              index[i].add(nnIdx[j]);
            }
            else break;
          }
          //cerr<<pos1[layer[i]][index[i]][0]<<" "
//<<pos1[layer[i]][index[i]][1]<<" "
//<<pos1[layer[i]][index[i]][2]<<endl;
      }
cerr<<"ANN DONE"<<endl;
//====================================================================
      delete kdTree;
      dataPts = annAllocPts(0,0);
      delete [] nnIdx;
      delete [] dists;
      queryPt = annAllocPt(0);
      annClose();                     // done with ANN
}
/*==============Near===============*/

/*===============Connect==========================*/
svClusterConnect::svClusterConnect(){

}

void svClusterConnect::ComputeConnect(svVector3Array *pos,  
                     svIntArray *cluster,
                     svIntArray *newcluster,
                     int maxCluster, int seed,
                     svVectorIntArray *nlayer,
                     svVectorIntArray *nindex){
 if(maxCluster==-1){
  return;
 }
//================================
 maxCluster++;
 vector< vector<int> > layer;
 vector< vector<int> > index;
 vector< int> clustersize;
 layer.resize(maxCluster);
 index.resize(maxCluster);
 for(int i=0;i<maxCluster;i++){
   clustersize.push_back(0);
 }
 
//=========split data===============
 for(int i=0;i<seed;i++){
  for(int j=0;j<pos[i].size();j++){
   if(cluster[i][j]<0) continue;
   layer[cluster[i][j]].push_back(i);
   index[cluster[i][j]].push_back(j);
   clustersize[cluster[i][j]]++;
  }
 }

//=========process================
for(int i=0;i<maxCluster;i++){
 queue<int> tlayer;
 queue<int> tindex;
 int label = 0; 
 while(layer[i].size()>0){
  tlayer.push(layer[i][0]);
  tindex.push(index[i][0]);
  while(tlayer.size()>0){
    int l = tlayer.front();
    int t = tindex.front();
    tlayer.pop();
    tindex.pop();
    newcluster[l][t] = label;
    for(int j=0;j<nlayer[l][t].size();j++){
     tlayer.push(nlayer[l][t][j]);
     tindex.push(nindex[l][t][j]);
    }
  }
  label++;
 }
}
//===========release=============
 clustersize.clear();
 for(int i=0;i<maxCluster;i++){
    layer[i].clear();
    index[i].clear();
 }
 layer.clear();
 index.clear();
}
/*===============================================*/

/*Symmetry*/
svSymmetry::svSymmetry()//svVectorField *inputfield)
{
}
svVector3 svSymmetry::CoordinateMatrix(svVector3 p,
                             svVector3 origin,
                             svVector3 x,
                             svVector3 y,
                             svVector3 z)
{
    svVector3 newp;
    newp = p - origin;

    svVector3Array M;
    M.add(x);
    M.add(y);
    M.add(z);

    svVector3 tmp = newp;
    newp[0] = tmp[0] * M[0][0]
            + tmp[1] * M[0][1]
            + tmp[2] * M[0][2];
    newp[1] = tmp[0] * M[1][0]
            + tmp[1] * M[1][1]
            + tmp[2] * M[1][2];
    newp[2] = tmp[0] * M[2][0]
            + tmp[1] * M[2][1]
            + tmp[2] * M[2][2];

    return newp;

}

int svSymmetry::GetType(svVector3 pos1, svVector3 end1, svScalar mag1,
                         svVector3 pos2, svVector3 end2, svScalar mag2,
                         svScalar angle_uncertain, svScalar mag_uncertain)
{
    bool f = false;

//    if(fabs(pos1[0]-7.77817)<1e-3
//     && fabs(pos1[1]-0)<1e-3
//     && fabs(pos1[2]+9.8995)<1e-3
//     && fabs(pos2[0]-7.77817)<1e-3
//     && fabs(pos2[1]-0)<1e-3
//     && fabs(pos2[2]-9.8995)<1e-3)
//      f = true;
//if(f)cerr<<mag1<<" "<<mag2<<" "<<mag_uncertain<<endl;
    if(fabs(mag1-mag2) > mag_uncertain)
           return -1;
//     if(f)cerr<<"i===test========"<<endl;

    if(fabs(mag1)<1e-10) return -1;
//     if(f)cerr<<"test"<<endl;

    if(fabs(mag2)<1e-10) return -1; 
//     if(f)cerr<<"test"<<endl;

  
    if(fabs(pos1[0] - pos2[0])<1e-3
     && fabs(pos1[1] - pos2[1])<1e-3
     && fabs(pos1[2] - pos2[2])<1e-3)
            return -1;
//     if(f)cerr<<"test"<<endl;

    if(fabs(pos1[0] - pos2[0])>1e-3)
           return -1;
//     if(f)cerr<<"test"<<endl;

    if(fabs(pos1[1] - pos2[1])>1e-3)
           return -1;
//     if(f)cerr<<"test"<<endl;

    if(fabs(pos1[2] + pos2[2])>1e-3)
           return -1;
//        if(f)cerr<<"======test======="<<endl;

    svVector3 end;
    double distance = (-pos1[2] + end1[2])*2.;

    int type = -1;
    double aa = 9e+9;

    end[0] = end1[0];
    end[1] = end1[1];
    end[2] = -end1[2];
    double d = dot(normalize(end-pos2), normalize(end2-pos2));
    if(d>1) d = 1.;
    double angle = fabs(acos(d));
    if(angle<angle_uncertain)
    {
         if(angle < aa)
         {
              type = _XSYSZS;
              aa = angle;
         }
    }

    end[0] = end1[0];
    end[1] = end1[1];
    end[2] = -end1[2]+distance;
    d = dot(normalize(end-pos2), normalize(end2-pos2));
    if(d>1) d = 1.;
    angle = fabs(acos(d));
    if(angle<angle_uncertain)
    {
         if(angle < aa)
         {  
              type = _XSYSZA;
              aa = angle;
         }
    }

    end[0] = end1[0];
    end[1] = -end1[1];
    end[2] = -end1[2];
    d = dot(normalize(end-pos2), normalize(end2-pos2));
    if(d>1) d = 1.;
    angle = fabs(acos(d));
    if(angle<angle_uncertain)
    {
         if(angle < aa)
         {  
              type = _XSYAZS;
              aa = angle;
         }
    }


    end[0] = end1[0];
    end[1] = -end1[1];
    end[2] = -end1[2]+distance;
    d = dot(normalize(end-pos2), normalize(end2-pos2));
    if(d>1) d = 1.;
    angle = fabs(acos(d));
    if(angle<angle_uncertain)
    {
         if(angle < aa)
         {  
              type = _XSYAZA;
              aa = angle;
         }
    }


    end[0] = -end1[0];
    end[1] = end1[1];
    end[2] = -end1[2];
    d = dot(normalize(end-pos2), normalize(end2-pos2));
    if(d>1) d = 1.;
    angle = fabs(acos(d));
    if(angle<angle_uncertain)
    {
         if(angle < aa)
         {  
              type = _XAYSZS;
              aa = angle;
         }
    }


    end[0] = -end1[0];
    end[1] = end1[1];
    end[2] = -end1[2]+distance;
    d = dot(normalize(end-pos2), normalize(end2-pos2));
    if(d>1) d = 1.;
    angle = fabs(acos(d));
    if(angle<angle_uncertain)
    {
         if(angle < aa)
         {  
              type = _XAYSZA;
              aa = angle;
         }
    }


    end[0] = -end1[0];
    end[1] = -end1[1];
    end[2] = -end1[2];
    d = dot(normalize(end-pos2), normalize(end2-pos2));
    if(d>1) d = 1.;
    angle = fabs(acos(d));
    if(angle<angle_uncertain)
    {
         if(angle < aa)
         {  
              type = _XAYAZS;
              aa = angle;
         }
    }


    end[0] = -end1[0];
    end[1] = -end1[1];
    end[2] = -end1[2]+distance;
    d = dot(normalize(end-pos2), normalize(end2-pos2));
    if(d>1) d = 1.;
    angle = fabs(acos(d));
//cerr<<d<<" "<<angle<<endl;
    if(angle<angle_uncertain)
    {
         if(angle < aa)
         {  
              type = _XAYAZA;
              aa = angle;
         }
    }
    
    return type;
}
void svSymmetry::SymmetryPair(svVector3 pos, svVector3 end, 
                    svVector3 &pair, svVector3 &pairend,
                    SYMMETRYTYPE type)
{
    double distance = -pos[2] + end[2];
    pair = pos;
    pair[2] = -pos[2];

    if(type == _XSYSZS)
    {
      pairend[0] = end[0];
      pairend[1] = end[1];
      pairend[2] = -end[2];
    }
    else if(type == _XSYSZA)
    {
       pairend[0] = end[0];
       pairend[1] = end[1];
       pairend[2] = -end[2]+distance*2.;
    }
    else if(type == _XSYAZS)
    {
       pairend[0] = end[0];
       pairend[1] = -end[1];
       pairend[2] = -end[2];
    }
    else if(type == _XSYAZA)
    {
      pairend[0] = end[0];
      pairend[1] = -end[1];
      pairend[2] = -end[2]+distance*2.;
    }
    else if(type == _XAYSZS)
    {
      pairend[0] = -end[0];
      pairend[1] = end[1];
      pairend[2] = -end[2];
    }
    else if(type == _XAYSZA)
    {
      pairend[0] = -end[0];
      pairend[1] = end[1];
      pairend[2] = -end[2] + distance*2.;
    }
    else if(type == _XAYAZS)
    {
      pairend[0] = -end[0];
      pairend[1] = -end[1];
      pairend[2] = -end[2];
    }
    else if(type == _XAYAZA)
    {
       pairend[0] = -end[0];
       pairend[1] = -end[1];
       pairend[2] = -end[2] + distance*2.;
    }
}
void svSymmetry::ComputeSymmetry(SymmetryProperty &property) //deposite in small programs!!!
{
//   property.angle_uncertain = 0.1;
//   property.mag_uncertain = 1e-10;

   cout<<"Symmetry processing ..."<<endl;
   char *str = new char[400];
char *outputfile[6];
//   char *symmetrystr = new char[200];
//   for(int j=0;j<200;j++) symmetrystr[j] = '\0';
//   sprintf(symmetrystr, "%s(%0.2f%0.2f%0.2f%0.2f%0.2f%0.2f)", symmetrystr,
//                  property.pos[0], property.pos[1], property.pos[2],
//                  property.dir[0], property.dir[1], property.dir[2]);
   sprintf(str,"%s", outputfile[0]);
///xsyszs%s.txt",
 //                property.outputdir, symmetrystr);
//cerr<<property.planepos[0]<<" "<<property.planepos[1]<<" "<<property.planepos[2]<<" "<<property.planedir[0]<<" "<<property.planedir[1]<<" "<< property.planedir[2]<<" "<<property.planedistance<<endl;
//cerr<<property.pos[0]<<" "<<property.pos[1]<<" "<<property.pos[2]<<" "<<property.dir[0]<<" "<<property.dir[1]<<" "<<property.dir[2]<<" "<<property.x[0]<<" "<<property.x[1]<<" "<<property.x[2]<<" "<<property.y[0]<<" "<<property.y[1]<<" "<<property.y[2]<<endl;
//cerr<<property.inputfile<<endl;
//cerr<<property.angle_uncertain<<" "<<property.mag_uncertain<<endl;
//  cerr<<str<<" "<<property.regenerate<<endl;
  ifstream test(str);
  if(!test.is_open())//||property.regenerate)
  {
   svVector3Array *pos;
   svVector3Array *end;
   svScalarArray *mag;

   char *inputfile;
   ifstream infile(inputfile);
   int n;
   infile>>n;
   pos = new svVector3Array[n];
   end = new svVector3Array[n];
   mag = new svScalarArray[n];
   vector< vector< vector<int> > > symmetry;   
   symmetry.resize(n);
//cerr<<"========================================================="<<endl;
   int N = 0;
   for(int i=0;i<n;i++)
   {
       int m;
       infile>>m;
       for(int j=0;j<m;j++)
       {
         svVector3 p,d;
         svScalar m;
         svInt a,r;
         infile>>p[0]>>p[1]>>p[2]
             >>d[0]>>d[1]>>d[2]>>m>>a>>r;
         
         svVector3 newp = CoordinateMatrix(p,
                             property.pos,
                             property.x,
                             property.y,
                             property.dir);
         svVector3 p2 = p + (m * (svScalar)(1e+20))* d; 
//cerr<<p[0]<<" "<<p[1]<<"======= "<<p[2]<<" "<<p2[0]<<" "<<p2[1]<<" "<<p2[2]<<endl;
         svVector3 newp2 = CoordinateMatrix(p2,
                             property.pos,
                             property.x,
                             property.y,
                             property.dir);

//         if(fabs(p[0]-1.5)<1e-3
//     && fabs(p[1]-12.5)<1e-3
//    && fabs(p[2]-0)<1e-3)
//          cerr<<newp[0]<<" "<<newp[1]<<" "<<newp[2]<<" "
//            <<newp2[0]<<" "<<newp2[1]<<" "<<newp2[2]<<endl;
//         if(fabs(p[0]+12.5)<1e-3
//     && fabs(p[1]+1.5)<1e-3
//    && fabs(p[2]-0)<1e-3)
//          cerr<<newp[0]<<" "<<newp[1]<<" "<<newp[2]<<" "
//            <<newp2[0]<<" "<<newp2[1]<<" "<<newp2[2]<<endl;

         pos[i].add(newp);
         end[i].add(newp2);
         mag[i].add(m* (svScalar)(1e+20));

         N++;
       }
   }
//cerr<<"========================================================="<<endl;

   svVector3 newplanepos = CoordinateMatrix(property.planepos,
                             property.pos,
                             property.x,
                             property.y,
                             property.dir);
   svVector3 tmpdir = property.planepos + property.planedir;
   svVector3 newplanedir = CoordinateMatrix(property.planedir,
                             property.pos,
                             property.x,
                             property.y,
                             property.dir);
   newplanedir = normalize(newplanedir - newplanepos);
//cerr<<"========================================================="<<endl;

   infile.close();
   int count[SYMMETRY_TYPE];
   for(int i=0;i<SYMMETRY_TYPE;i++)
         count[i] = -1;
   cerr<<"Processing symmetry ..."<<endl;
   for(int i=0;i<n;i++)
   {
      cerr<<i<<" ";
     // cerr<<i<<" ";
      symmetry[i].resize(pos[i].size()); 
      #pragma omp parallel for
      for(int j=0;j<pos[i].size();j++)
      {
        int symmetryindex[2*SYMMETRY_TYPE];

        svVector3 tmppos;
        tmppos[0]= pos[i][j][0];
        tmppos[1]= pos[i][j][1];
        tmppos[2]= -pos[i][j][2];
        svScalar distance = dot(tmppos-newplanepos, newplanedir);
        for(int t=0;t<SYMMETRY_TYPE;t++)
        {
             symmetryindex[t*2] = -1;
             symmetryindex[t*2+1]=-1;
        } 
        int whole_count = 0;
        int layer = distance/property.planedistance;

//    if(fabs(pos[i][j][0]-7.77817)<1e-3
//     && fabs(pos[i][j][1]-0)<1e-3
//     && fabs(pos[i][j][2]-9.8995)<1e-3)
//         cerr<<distance<<" "<<property.planedistance<<" "<<layer<<endl;
        if(layer >=0 && layer<n)
        {
           for(int tt=0;tt<pos[layer].size();tt++)
           {
         //      if(whole_count == SYMMETRY_TYPE)break;
               int type = GetType(pos[i][j], end[i][j], mag[i][j],
                       pos[layer][tt], end[layer][tt], mag[layer][tt],
                       property.angle_uncertain, property.mag_uncertain*1e+20);
               if(type<0)
               {
                     continue;
               }
               symmetryindex[type*2]=layer;
               symmetryindex[type*2+1]=tt;
               count[type]++;
               whole_count++;
           }
        }
        for(int t=0;t<SYMMETRY_TYPE;t++)
        {
            symmetry[i][j].push_back(symmetryindex[t*2]);
            symmetry[i][j].push_back(symmetryindex[t*2+1]);
        }
      }

   }cerr<<endl;
//-----------savetofile-----------------
   for(int tt=0;tt<SYMMETRY_TYPE;tt++)
   {
     char *str2 = new char[400];
//     switch(tt)
//     {
//        case 0:  sprintf(str2,"%s/xsyszs%s.txt",
//                     property.outputdir, symmetrystr);break;
//        case 1:  sprintf(str2,"%s/xsysza%s.txt",
//                     property.outputdir, symmetrystr);break;
//        case 2:  sprintf(str2,"%s/xsyazs%s.txt",
//                     property.outputdir, symmetrystr);break;
//        case 3:  sprintf(str2,"%s/xsyaza%s.txt",
//                     property.outputdir, symmetrystr);break;
//        case 4:  sprintf(str2,"%s/xayszs%s.txt",
//                     property.outputdir, symmetrystr);break;
//        case 5:  sprintf(str2,"%s/xaysza%s.txt",
//                     property.outputdir, symmetrystr);break;
//        case 6:  sprintf(str2,"%s/xayazs%s.txt",
//                     property.outputdir, symmetrystr);break;
//        case 7:  sprintf(str2,"%s/xayaza%s.txt",
//                     property.outputdir, symmetrystr);break;
//     }  
     sprintf(str2, "%s", outputfile[tt]);
    ofstream outfile(str2);
     outfile<<count[tt]+1<<endl;
     for(int i = 0;i<n;i++)
     {
         for(int j=0;j<pos[i].size();j++)
         {
             if(symmetry[i][j][tt*2]>=0)
                outfile<<2<<" ";
             else 
                outfile<<1<<" ";
             outfile<<i<<" "<<j;
             if(symmetry[i][j][tt*2]>=0)
                 outfile<<" "<<symmetry[i][j][tt*2]<<" "<<symmetry[i][j][tt*2+1];
             outfile<<endl;
         }
     }
     outfile.close();
     delete [] str2;
  }

  for(int i=0;i<n;i++)
  {
    for(int j=0;j<pos[i].size();j++)
    {
          symmetry[i][j].clear();
    }
    symmetry[i].clear();
  }   
  symmetry.clear();

  for(int i=0;i<n;i++)
  {  pos[i].free();
     end[i].free();
     mag[i].free();
   }

   delete [] end;
   delete [] pos;
   delete [] mag;
   delete [] str;

cerr<<"========================================================="<<endl;

  }
  else 
    test.close();
cerr<<"Symmetry done..."<<endl;
}

void svSymmetry::ComputeSymmetry(SymmetryProperty &property, 
                                svQDOT *myQDOT, svVector3 planepos, 
                                svVector3 planedir, 
                                char *dir) //deposite in small programs!!!
{
  cout<<"Symmetry processing ..."<<endl;
  char *str = new char[400];
  sprintf(str, "%s/xsyszs%0.2f%0.2f%0.2f%0.2f%0.2f%0.2f.txt", dir,
               property.pos[0], property.pos[1], property.pos[2],
               property.dir[0], property.dir[1], property.dir[2]);
  cerr<<str<<endl;
  ifstream test(str);
  if(test.is_open()) {test.close(); return;}

//============================Cooridnate Transformation==============//
  svVector3Array *pos;
  svVector3Array *end;
  svScalarArray *mag;
  svIntArray *mapIndex;
  int n = (myQDOT->rbbox[2] - myQDOT->lbbox[2])/myQDOT->minZDistance+1;
  pos = new svVector3Array[n];
  end = new svVector3Array[n];
  mag = new svScalarArray[n];
  mapIndex = new svIntArray[n];
  vector< vector< vector<int> > > symmetry;
  symmetry.resize(n);
  int N = myQDOT->qdotData.size();
  for(int i=0;i<myQDOT->qdotData.size();i++){
    svVector3 p =myQDOT->qdotData[i].pos;
    svVector3 d = myQDOT->qdotData[i].dir;
    svScalar m = myQDOT->qdotData[i].den;
    svVector3 newp = CoordinateMatrix(p,
                             property.pos,
                             property.x,
                             property.y,
                             property.dir);
    svVector3 p2 = p + (m * (svScalar)(1e+20))* d;
    svVector3 newp2 = CoordinateMatrix(p2,
                             property.pos,
                             property.x,
                             property.y,
                             property.dir);
    int index = (p[2]-myQDOT->lbbox[2])/myQDOT->minZDistance;
    pos[index].add(newp);
    end[index].add(newp2);
    mag[index].add(m* (svScalar)(1e+20));
    mapIndex[index].add(i);
  }
  svVector3 newplanepos = CoordinateMatrix(planepos,
                             property.pos,
                             property.x,
                             property.y,
                             property.dir);
  svVector3 tmpdir = planepos + planedir;
  svVector3 newplanedir = CoordinateMatrix(planedir,
                             property.pos,
                             property.x,
                             property.y,
                             property.dir);
  newplanedir = normalize(newplanedir - newplanepos);
//=================================================================//
  int count[SYMMETRY_TYPE];
  for(int i=0;i<SYMMETRY_TYPE;i++) count[i] = -1;
   cerr<<"Processing symmetry ..."<<endl;
   for(int i=0;i<n;i++){
      cerr<<i<<" ";
      symmetry[i].resize(pos[i].size()); 
      #pragma omp parallel for
      for(int j=0;j<pos[i].size();j++){
        int symmetryindex[2*SYMMETRY_TYPE];
        svVector3 tmppos;
        tmppos[0]= pos[i][j][0];
        tmppos[1]= pos[i][j][1];
        tmppos[2]= -pos[i][j][2];
        svScalar distance = dot(tmppos-newplanepos, newplanedir);
        for(int t=0;t<SYMMETRY_TYPE;t++){
             symmetryindex[t*2] = -1;
             symmetryindex[t*2+1]=-1;
        } 
        int whole_count = 0;
        int layer = distance/myQDOT->minZDistance;
        if(layer >=0 && layer<n){
           for(int tt=0;tt<pos[layer].size();tt++){
               int type = GetType(pos[i][j], end[i][j], mag[i][j],
                       pos[layer][tt], end[layer][tt], mag[layer][tt],
                       property.angle_uncertain, property.mag_uncertain*1e+20);
               if(type<0) continue;
               symmetryindex[type*2]=layer;
               symmetryindex[type*2+1]=tt;
               count[type]++;
               whole_count++;
           }
        }
        for(int t=0;t<SYMMETRY_TYPE;t++){
            symmetry[i][j].push_back(symmetryindex[t*2]);
            symmetry[i][j].push_back(symmetryindex[t*2+1]);
        }
      }
   }cerr<<endl;
//-----------savetofile-----------------
  svVector3 cutPos = property.pos; svVector3 cutDir = property.dir;
   for(int tt=0;tt<SYMMETRY_TYPE;tt++){
     char *str2 = new char[400];
     switch(tt){
      case 0: sprintf(str2, "%s/xsyszs%0.2f%0.2f%0.2f%0.2f%0.2f%0.2f.txt",
                           dir, cutPos[0], cutPos[1], cutPos[2],
                           cutDir[0], cutDir[1], cutDir[2]); break;
      case 1: sprintf(str2, "%s/xsysza%0.2f%0.2f%0.2f%0.2f%0.2f%0.2f.txt",
                           dir, cutPos[0], cutPos[1], cutPos[2],
                           cutDir[0], cutDir[1], cutDir[2]); break;
      case 2: sprintf(str2, "%s/xsyazs%0.2f%0.2f%0.2f%0.2f%0.2f%0.2f.txt",
                           dir, cutPos[0], cutPos[1], cutPos[2],
                           cutDir[0], cutDir[1], cutDir[2]); break;
      case 3: sprintf(str2, "%s/xsyaza%0.2f%0.2f%0.2f%0.2f%0.2f%0.2f.txt",
                           dir, cutPos[0], cutPos[1], cutPos[2],
                           cutDir[0], cutDir[1], cutDir[2]); break;
      case 4: sprintf(str2, "%s/xayszs%0.2f%0.2f%0.2f%0.2f%0.2f%0.2f.txt",
                           dir, cutPos[0], cutPos[1], cutPos[2],
                           cutDir[0], cutDir[1], cutDir[2]); break;
      case 5: sprintf(str2, "%s/xaysza%0.2f%0.2f%0.2f%0.2f%0.2f%0.2f.txt",
                           dir, cutPos[0], cutPos[1], cutPos[2],
                           cutDir[0], cutDir[1], cutDir[2]); break;
      case 6: sprintf(str2, "%s/xayazs%0.2f%0.2f%0.2f%0.2f%0.2f%0.2f.txt",
                           dir, cutPos[0], cutPos[1], cutPos[2],
                           cutDir[0], cutDir[1], cutDir[2]); break;
      case 7: sprintf(str2, "%s/xayaza%0.2f%0.2f%0.2f%0.2f%0.2f%0.2f.txt",
                           dir, cutPos[0], cutPos[1], cutPos[2],
                           cutDir[0], cutDir[1], cutDir[2]); break;
     }  
     ofstream outfile(str2);
     outfile<<count[tt]+1<<endl;
     for(int i = 0;i<n;i++){
         for(int j=0;j<pos[i].size();j++){
             if(symmetry[i][j][tt*2]>=0)
                outfile<<2<<" ";
             else 
                outfile<<1<<" ";
             outfile<<mapIndex[i][j];
             if(symmetry[i][j][tt*2]>=0)
                 outfile<<" "<<mapIndex[symmetry[i][j][tt*2]][symmetry[i][j][tt*2+1]];
             outfile<<endl;
         }
     }
     outfile.close();
     delete [] str2;
  }
//============================release memory=======================//
  for(int i=0;i<n;i++)
  {
    for(int j=0;j<pos[i].size();j++)
    {
          symmetry[i][j].clear();
    }
    symmetry[i].clear();
  }   
  symmetry.clear();

  for(int i=0;i<n;i++)
  {  pos[i].free();
     end[i].free();
     mag[i].free();
     mapIndex[i].free();
   }
   delete [] mapIndex;
   delete [] end;
   delete [] pos;
   delete [] mag;
   delete [] str;
cerr<<"========================================================="<<endl;
}
/*Symmetry end*/


//=======================ContourTree=========================
svContourTree::svContourTree()//svVectorField *inputField)
{
}

void svContourTree::ComputeContourTree(char *dir, char *planename, int seed_num)//ContourTreeProperty &property)
{
   char *exe = new char[2048];
   char *input = new char[400];
   char *output = new char[400];
   for(int i=0;i<seed_num;i++)
   {
      sprintf(input, "%s/density%s%d.txt", dir, planename,i);
      sprintf(output,"%s/contourtree%s%d.txt", dir, planename, i);
      ifstream infile(output);
      if(!infile.is_open())
      {
        sprintf(exe, "%s/ContourTree %s %s",  BIN_DIR,
              input, output);
        system(exe);
      }
      else
      {
        infile.close();
      }
   }
   delete [] input;
   delete [] output;
   delete [] exe;
}
//=======================ContourTree end=====================

//==================Mesh=========================
void svMeshGenerator::GenerateMesh(char *ifname, char *ofname, float sbottom, float pbottom){
  ifstream infile(ofname);
  if(infile.is_open()){infile.close(); return;}
//sbottom: bottom of semisphere
//pbottom: bottom of plane
  char *exe = new char[1024];
  sprintf(exe, "%s/GenerateMesh %0.2f %0.2f %s %s", BIN_DIR,
                sbottom, pbottom, ifname, ofname);
  system(exe);
//  cerr<<exe<<endl;
  delete [] exe;
}

void svMeshGenerator::GenerateBoundaryPoints(char *inputfile, char *outputfile){
    char *exe = new char[1024];
    sprintf(exe, "%s/ex_alpha_shapes_3 %s > %s 2>&1", BIN_DIR, inputfile, outputfile);
    system(exe);
    //cerr<<exe<<endl;
    sprintf(exe, "rm %s", inputfile);
    system(exe);
    delete [] exe;
}

//==================Mesh End=====================
}
