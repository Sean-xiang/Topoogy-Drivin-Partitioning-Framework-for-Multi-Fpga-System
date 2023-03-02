
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include<string>
using namespace std ;
struct Input {
int TotalFpga ;
int TotalFchannel ;
int CapaciF;
int TotalNodes;
int Totalnets;
int TotalFixnodes ;
};
static   int MaxDistance = 99999; // Can't arrive
struct Fpcut {   // use for priority Queue Ri
  int fpga ;
  int cutsize ;
};

struct Recordrecover{
  int node ;
  int fpga ;
  int cutsize ;
  int priorityRindex ;
  int priorityQindex ;
};
struct NodeFpcut{
  vector <Fpcut> cutfp ;
};
struct nodeCandidate {   // use for priority Queue Ri
  int node ;
  int numofCandidate ;
};

struct CNode{
  vector<int> neighbors ;
  int site;
};

struct FNode{
  vector<int> neighbors ;
  vector<int> circuitnodes ;
};

struct bigdata {
  int node ;
  int neighbornum ;
};

void GetLine( vector<int> &output , FILE *fp ) {
    string temp = "/0";
    char tempch  ;
    int a = 0  ;
    fscanf( fp,"%c", &tempch ) ;
    while ( tempch != '\n') {
      if ( tempch != ' ' && tempch != '\n') {
        temp += tempch ;
      } // if
      else if (tempch == ' ') {
        a = atoi(temp.c_str()) ;
        output.push_back(a) ;
        temp = "" ;
      } // elseif
      else {
      } // ??? /n

      if ( fscanf( fp,"%c", &tempch ) == EOF )
        break ;
    } // while
    if ( temp != "") {
      a = atoi(temp.c_str()) ;
      output.push_back(a) ;
      temp = "" ;
    } // if
}

class FpgaGrah{
  public :
    vector <CNode> GCnode ;
    vector <int> fixnode;
    vector <FNode> GFnode ;
    vector<int> FCapacity ; // 0~N fpga's capacity
    vector<vector<int> > Cddt ; //Cddt[i] means which fpga that ith node can put in
    FpgaGrah( Input input,FILE *fp  ) {
      vector<int> Line ;
      vector<int> tempv ;

      for( int i = 0  ; i  < input.TotalFpga; i++  ) {
        tempv.push_back(i) ;
        FNode temp ;
        FCapacity.push_back( input.CapaciF ) ;
        GFnode.push_back(temp) ;
      } // for
      for( int i = 0  ; i  < input.TotalFchannel; i++  ) {
        GetLine( Line, fp ) ;
        GFnode[Line[0]].neighbors.push_back( Line[1]) ; // line[0] Line[1] = fpga fpga
        GFnode[Line[1]].neighbors.push_back( Line[0]) ;
        Line.erase(Line.begin(),Line.end() ) ;
      } // for
      for( int i = 0  ; i  < input.TotalNodes; i++  ) {
        CNode tempc ;
        GCnode.push_back(tempc) ;
        Cddt.push_back(tempv) ;  // tempv?]?t???fpga??number ??l??Cddt?O??????FPGA???i?H??
      } // for

      for( int i = 0  ; i  < input.Totalnets; i++  ) { // ??net
        GetLine( Line, fp ) ;
        for ( int j = 1 ; j < Line.size() ; j++ ) {
          GCnode[Line[0]].neighbors.push_back( Line[j]) ;
          GCnode[Line[j]].neighbors.push_back( Line[0]) ;
        } // for
        Line.erase(Line.begin(),Line.end() ) ;
      } // for
      for( int i = 0  ; i  < input.TotalFixnodes; i++  ) { // ??Fixnode ??J FPGA
        GetLine( Line, fp ) ;   // ????Line[1]??fpga?@?w?n?? Line[0] ??node
        GFnode[Line[1]].circuitnodes.push_back(Line[0]) ;
        GCnode[Line[0]].site = Line[1] ;
        fixnode.push_back(Line[0]) ;
        Cddt[Line[0]].erase( Cddt[Line[0]].begin(),Cddt[Line[0]].end() ) ; // ?T?wnode??fpga?u???@??
        Cddt[Line[0]].push_back(Line[1]) ;  // ?T?wnode??fpga?u???@??
        FCapacity[Line[1]]-- ;          // ??J??FCapacity -1
        Line.erase(Line.begin(),Line.end() ) ;
      } // for

    } // ???


   // -----------------------------algorithm1 ---------------------------------------

    vector<vector<int> > AllFDistance() { // ??X???Fpga???Z???B?O?@??x?}
      int Fsize = GFnode.size() ;
      vector<vector<int> > adjancymatrix ;
      adjancymatrix.resize(Fsize) ;
      int max = 0 ;
      for (int i = 0; i < Fsize; i++) {
        adjancymatrix[i].resize(Fsize, MaxDistance);
        for (int j = 0; j < Fsize; j++) {
            if (i == j){
                adjancymatrix[i][j] = 0;
            } // if
        } // for
      } // for
      for ( int i = 0 ; i < Fsize ; i++ ) {
        for ( int j = 0 ; j< GFnode[i].neighbors.size() ; j++ ) {
          adjancymatrix[i][GFnode[i].neighbors[j]] = 1;   // ??ladjancymatrix
        } //for
      } // for
      for (int k = 0; k < Fsize; k++) {          // floyd warshall ??Xall pair shoretest path
        for (int i = 0; i < Fsize; i++) {
            for (int j = 0; j < Fsize; j++) {
                if ((adjancymatrix[i][j] > adjancymatrix[i][k]+adjancymatrix[k][j])
                     && (adjancymatrix[i][k] != MaxDistance)) {
                    adjancymatrix[i][j] = adjancymatrix[i][k]+adjancymatrix[k][j];
                } // if
            } // for
        } // for
      }  // for

     return adjancymatrix ;
    } // AllFDistance()
    vector<vector<int> > AllCDistance() { // ??X???Fpga???Z???B?O?@??x?}
      int Csize = GCnode.size() ;
      vector<vector<int> > adjancymatrix ;
      adjancymatrix.resize(Csize) ;
      int max = 0 ;
      for (int i = 0; i < Csize; i++) {
        adjancymatrix[i].resize(Csize, MaxDistance);
        for (int j = 0; j < Csize; j++) {
            if (i == j){
                adjancymatrix[i][j] = 0;
            } // if
        } // for
      } // for
      for ( int i = 0 ; i < Csize ; i++ ) {
        for ( int j = 0 ; j< GCnode[i].neighbors.size() ; j++ ) {
          adjancymatrix[i][GCnode[i].neighbors[j]] = 1;   // ??ladjancymatrix
        } //for
      } // for
      for (int k = 0; k < Csize; k++) {          // floyd warshall ??Xall pair shoretest path
        for (int i = 0; i < Csize; i++) {
            for (int j = 0; j < Csize; j++) {
                if ((adjancymatrix[i][j] > adjancymatrix[i][k]+adjancymatrix[k][j])
                     && (adjancymatrix[i][k] != MaxDistance)) {
                    adjancymatrix[i][j] = adjancymatrix[i][k]+adjancymatrix[k][j];
                } // if
            } // for
        } // for
      }  // for

     return adjancymatrix ;
    } // AllFCistance()

    bool isfixed ( int node ) {
     for ( int i = 0 ; i < fixnode.size() ; i++ ) {
       if( node == fixnode[i] )
         return true;
     } // for
     return false ;
   } // isfixed
    void SetCandidate() { // ?]?w???node ??Candidate
       vector<vector<int> > Fallpairdistance = AllFDistance() ;
       int maxFdist = 0 ;
       vector<vector<int> > Callpairdistance = AllCDistance() ;
       int maxCdist = 0 ;
       int nowF = 0 ;
       int nowC = 0 ;
       int disij = 0 ;
       vector<int>  availablec ;  // ?Z???{?b?o??node???Z?? < ??fpga????Lfpga??j?Z?????I
       vector<int>  fixpairq ;
       for ( int i = 0 ; i < fixnode.size() ; i ++  ) {
        fixpairq.push_back(fixnode[i]) ;
       }
       vector<int>  availablef ;
       while ( fixpairq.size() > 0 ) {
         nowC = fixpairq[0] ;
         nowF = GCnode[nowC].site ; // ?{?b?o??fixnode??b????FPGA
         maxFdist = GetMax(Fallpairdistance, GFnode.size(),nowF,MaxDistance ) ;
         maxCdist = GetMax(Callpairdistance, GCnode.size(),nowC,MaxDistance ) ;
         for ( int i = 0 ; i < Callpairdistance[0].size() ; i ++ ) {
           if ( Callpairdistance[nowC][i] < maxFdist  ) { // S(Vi,d)
             if ( !isfixed(i) && i != nowC ) { // movable node and ???O??v
               availablec.push_back( i ) ;  // �ݭn�Q�B�z��Circuit
             }
           } // if
         } // for

         for ( int j = 0 ; j < availablec.size(); j++ ) {
            disij = Callpairdistance[nowC][availablec[j]] ;
            SetavailableF( Fallpairdistance[nowF], availablef, disij ) ; // availablef =  S'( Vj',disij )
            Cddt[availablec[j]] = Operationand( Cddt[availablec[j]],availablef ) ;
            if ( Cddt[availablec[j]].size() == 1 ){
              fixpairq.push_back(availablec[j]) ;
              GCnode[availablec[j]].site = Cddt[availablec[j]][0] ;  // site can't move
              fixnode.push_back(availablec[j]) ;
              FCapacity[Cddt[availablec[j]][0]]-- ;
            } // also a fixnode
            if ( Cddt[availablec[j]].size() == 0 ) {
              cout << "There is no feasible solution" << endl ;
            }
           availablef.erase( availablef.begin(),availablef.end()) ;
         }
         availablef.erase( availablef.begin(),availablef.end()) ;
         availablec.erase( availablec.begin(),availablec.end()) ;
         disij = 0 ;
         fixpairq.erase(fixpairq.begin()) ;
       }  // while
    } // SetCandidate()


    void SetavailableF ( vector<int> Fdistance , vector<int> &availableF, int dis ) {
       for ( int i = 0 ; i < Fdistance.size() ; i++ ) {
          if ( Fdistance[i] <= dis ) {
              availableF.push_back(i) ;
          } //if
       } // for
    } // SetavailableF

    vector <int> Operationand( vector<int> a , vector<int> b ) {
        vector<int> result ;
        for( int i = 0 ; i < a.size() ; i++ ) {
          for ( int j = 0 ; j < b.size(); j++ ) {
            if(a[i] == b[j]) {
              result.push_back(a[i]) ;
             }
          }
        }
        return result ;
    }

    int GetMax(vector<vector<int> > adjancymatrix, int Fsize,int whichobject,int MaxDistance ) {
      int max = 0 ;
      for ( int i = 0 ; i < Fsize ; i++ ) {
        if ( max < adjancymatrix[i][whichobject] && adjancymatrix[i][whichobject] != MaxDistance )
           max = adjancymatrix[i][whichobject] ;
      } // for
      return max ;
    } //GetMax()

    void dbug( vector<vector<int> > adjancymatrix , int Fsize ) {

      for (int i = 0; i < Fsize; i++) {
        for (int j = 0; j < Fsize; j++) {
           cout << adjancymatrix[i][j] << " " ;
        } // for
           cout << endl ;
      } // for
    } // dbug

   // -----------------------------algorithm1 ---------------------------------------
   // -----------------------------algorithm2 for size smaill---------------------------------------



  void InitialPartion(string outname ) {
    vector<NodeFpcut> priorityR ; // priorityR[i].cutfp[0].fpga  or cutsize  -> node i's 0th fpga and its cut size
    priorityR.resize( GCnode.size() ); // numof C's fpga and its cut size and each node's fpga site will sort in cut size small->big
    Fpcut tempfp ;
    vector<int> result ; // result[i] = a -> ith node in ath fpga = part
    result.resize(GCnode.size(), -1 ) ; // if result[i] = -1 , imply ith node doesn't put in
    vector<nodeCandidate>  priorityQ ; // put node and num of its Candidate
    vector <Recordrecover>  recover ; // recoverCddt[i][j] for ith node 's jth fpga in this round is removed ,0 imply this round don't remove, 1 imply this round remove
    bool traceback = false ;
    int round = 0 ;
    nodeCandidate tempnode ;
    int nowpriorityQ = 0 ;
    int nowC = -1 ;  // which node is currently reference
    int nowF = -1 ; // which fpga is currently reference

    for ( int i = 0 ; i < fixnode.size() ; i++ ) {
      result[fixnode[i]] = GCnode[fixnode[i]].site ; // fixnode?��result don't move
    } // for
    for ( int j = 0 ; j < Cddt.size() ; j++ ) {   // Add to Q
      if( !isfixed(j)){
        tempnode.node = j ;
        tempnode.numofCandidate = Cddt[j].size() ;
        priorityQ.push_back(tempnode) ;  // Q has only fixed node, R has fixed and nod fixed ,
      } // if
      for ( int k = 0 ; k < Cddt[j].size() ; k++) { //Cddt[j][k] menas jth node's kth candidate
         tempfp.fpga = Cddt[j][k] ;
         tempfp.cutsize = CauculateCutsize( Cddt[j][k], j, result ) ;
         priorityR[j].cutfp.push_back( tempfp ); //
      } // for
    } // for

    selection_sortQ( priorityQ, priorityQ.size() ) ; // put numofCandidate small->big  There are no fixnodes in priority Q

    for( int a = 0 ; a < priorityR.size() ; a ++ ) {
      selection_sortR( priorityR[a], priorityR[a].cutfp.size() ) ; // howmany fp and its cutsize
    } // for

    while ( priorityQ.size() > 0 ) {
      round++ ;
      cout << "round:  " << round << "priorityqsize : " << priorityQ.size()  <<endl ;
      nowC = priorityQ[0].node ;
      for( int i = 0 ; i < priorityR[nowC].cutfp.size(); i++ ) {
         priorityR[nowC].cutfp[i].cutsize =CauculateCutsize( priorityR[nowC].cutfp[i].fpga, nowC, result ) ;
      }  // for
      selection_sortR( priorityR[nowC], priorityR[nowC].cutfp.size() );
      nowpriorityQ = priorityQ[0].numofCandidate ;
      priorityQ.erase(priorityQ.begin() ) ; // get first node
      if ( FCapacity[priorityR[nowC].cutfp[0].fpga] > 0 ) { // this fpga could put
        nowF = priorityR[nowC].cutfp[0].fpga ;
      } // if
      else { // this Fpga is full
        for ( int x = 1 ; x <  priorityR[nowC].cutfp.size(); x++  ) { // find next could put in fpga
          if ( FCapacity[priorityR[nowC].cutfp[x].fpga] > 0  )  // this fpga could put
            nowF = priorityR[nowC].cutfp[x].fpga ;
        } // for
      } // else
      FCapacity[nowF] -- ;
      result[nowC] = nowF ; // put in temprary
      for( int i = 0 ; i < GCnode[nowC].neighbors.size() ; i++ ) {
        if ( !Update( nowF,GCnode[nowC].neighbors[i], Cddt, priorityR, recover, priorityQ) ) {  // Update priorityR[nodeC's neighbors] and Cddt[nodeC's neighbors],
          traceback = true ;  // this update is not legal
          break ;
        } // if

      } // for

      if ( traceback ) { // need recover
        RemoveandRecoverall( nowF,nowC, Cddt, priorityR, priorityQ,recover) ; //Remove nowC's Cddt  and update nowC neighbors 's priorityR priorityQ3
        nowpriorityQ-- ; // nowF Can't put so we need to find next and the Candidatenum would -1
        if ( nowpriorityQ > 0 ) {
        	tempnode.node = nowC ;
        	tempnode.numofCandidate = nowpriorityQ ;

        	priorityQ.insert( priorityQ.begin(),tempnode ) ;
		}
        FCapacity[nowF] ++ ; // recover nowC's neighbor's   priorityR and priorityQ and Cddt, after recover the recovercutsize and recoverCddt need to initialize
        recover.clear() ;

        result[nowC] = -1 ;
      } // if
      else { //don't need recover
        /*for ( int i = 0 ; i < priorityR.size() ; i++ ) {
          for ( int j = 0 ; j < priorityR[i].cutfp.size() ; j++ ) {
            priorityR[i].cutfp[j].cutsize = CauculateCutsize(priorityR[i].cutfp[j].fpga, i, result ) ; //
          } // for
        } // for*/
        selection_sortQ( priorityQ, priorityQ.size() ) ; // put numofCandidate small->big  There are no fixnodes in priority Q(sort need improve )

        /*for( int a = 0 ; a < priorityR.size() ; a ++ ) {
          if ( result[a] == -1 ) // not put in yet
            selection_sortR( priorityR[a], priorityR[a].cutfp.size() ) ; // howmany fp and its cutsize
        } // for*/

        recover.clear() ;
      } // else         need update every node's cutsize and sortQ and sort R

      traceback = false ;

    } // while
    for ( int i = 0 ; i < result.size() ; i ++ ) {
      if ( result[i] == -1 ) {
        for ( int j = 0 ; j < FCapacity.size() ; j ++ ) {
          if ( FCapacity[j] > 0 ) {
            FCapacity[j] -- ;
            result[i] = j ;
          } //if
	    } // for
      } // if
	} // for
	Outfile( result, outname ) ;
 } // InitialPartion()


  void RemoveandRecoverall( int nowF,int nowC,vector<vector<int> > &Cddt,  vector<NodeFpcut> & priorityR, vector<nodeCandidate>  &priorityQ, vector <Recordrecover> recover ) {
    int Cddtindex = -1 ;
    Cddtindex = GetCddtindex( Cddt[nowC],nowF ) ;
    Cddt[nowC].erase(Cddt[nowC].begin() + Cddtindex ) ; // remove nowF in nowC
    Fpcut temp ;
    for ( int i = 0 ; i < recover.size(); i++ ) {
      if ( recover[i].priorityQindex != -1 ) { // this node has already put in
        temp.cutsize = recover[i].cutsize ;
        temp.fpga = recover[i].fpga  ;
        if (recover[i].priorityRindex > priorityR[recover[i].node].cutfp.size() )
          priorityR[recover[i].node].cutfp.push_back(temp) ;
        else
          priorityR[recover[i].node].cutfp.insert( priorityR[recover[i].node].cutfp.begin() + recover[i].priorityRindex,temp ) ;
        Cddt[recover[i].node].push_back(recover[i].fpga ) ;
        priorityQ[recover[i].priorityQindex].numofCandidate++ ;
      } //if
    } // for

  } // RemoveandRecoverall()

  int GetCddtindex( vector<int>Cddt, int nowF ) {
    int result = 0 ;
    for ( int i = 0 ; i < Cddt.size() ; i++ ) {
      if ( Cddt[i] == nowF )
        return i ;
    } // for
    return -1 ;
  } // GetCddtindex()
  bool Update ( int nowF,int nodeC ,  vector<vector<int> > &Cddt, vector<NodeFpcut> & priorityR, vector <Recordrecover>  &recover  ,
              vector<nodeCandidate>  &priorityQ ) {  // nodeC is the neighbor's node number
    bool legal = false ; // its neighbors can put in this fpga
    int priorityRindex = 0 ;
    int priorityQindex = 0 ;
    Recordrecover temprecover;
    for ( int i = 0 ; i < Cddt[nodeC].size() ; i++ ) {
      for ( int j = 0 ; j < GFnode[nowF].neighbors.size() ; j++ ) {
         if (  Cddt[nodeC][i] ==  GFnode[nowF].neighbors[j] || Cddt[nodeC][i] == nowF  )
           legal = true ;   // imply Cddt[nodeC][j] nodeC's jth fpga can put ,otherwise Cddt[nodeC]  nodeC's jth fpga not near the nowF , so it shoud be removed,topology constrant
      } // for
      if (!legal){
        temprecover.node = nodeC ;  //  ith NODE  put in jth fpga is not legal
        temprecover.fpga = Cddt[nodeC][i] ;
        priorityRindex = GetPriorityRindex( priorityR[nodeC], Cddt[nodeC][i]) ; //get priorityR's fpga index
        temprecover.priorityRindex = priorityRindex ;
        temprecover.cutsize = priorityR[nodeC].cutfp[priorityRindex].cutsize ; // ith node in jth fpga's cut size before removed
        priorityQindex = GetPriorityQindex( priorityQ, nodeC ) ; //get priorityQ's node index
        temprecover.priorityQindex = priorityQindex ;
        if (priorityQindex != -1 )
          priorityQ[priorityQindex].numofCandidate-- ;  // update priorityQ
        priorityR[nodeC].cutfp.erase( priorityR[nodeC].cutfp.begin() + priorityRindex ) ; //update priorityR
        recover.push_back( temprecover) ;
        Cddt[nodeC].erase( Cddt[nodeC].begin() + i ) ; // remove Cddt?????A

      } // if
      if( Cddt[nodeC].size() == 0  )
        return false ; // this update isn legal and need recover , because neighbor node has no fpga to put in

      legal = false ;
    } // for
    return true ;
  } // Update

  int GetPriorityQindex( vector<nodeCandidate>  &priorityQ , int nodenum ) {
    for ( int i = 0 ; i < priorityQ.size() ; i++ ) {
      if ( priorityQ[i].node == nodenum )
        return i ;
    }
      return -1 ;
  } // GetPriorityQindex
  int GetPriorityRindex( NodeFpcut priority , int numberfpga ) {
    for ( int i = 0 ; i < priority.cutfp.size() ; i++ ) {
      if ( priority.cutfp[i].fpga == numberfpga)
        return i ;
    }
      return -1 ;
  } // GetPriorityRindex

  bool isfnext(int fixnode, int sitefpga) {
   for( int j = 0 ; j < GFnode[sitefpga].neighbors.size(); j++ ) {
        if ( GFnode[sitefpga].neighbors[j] == fixnode)
          return true ;
   } // for

   return false ;
  } // is fnext ;
  int CauculateCutsize( int sitefpga , int node, vector <int > result  ) {  // set when node put it  in  which site and what is it's cutsize  happen
    int cutsize = 0  ;
    for ( int i = 0 ; i < GCnode[node].neighbors.size(); i++ ) {
      if ( sitefpga != result[GCnode[node].neighbors[i]] &&  result[GCnode[node].neighbors[i]] != -1 && isfnext( result[GCnode[node].neighbors[i]],sitefpga) ) {
        cutsize++ ;
      }
      else if ( sitefpga != result[GCnode[node].neighbors[i]] &&  result[GCnode[node].neighbors[i]] != -1 && !isfnext( result[GCnode[node].neighbors[i]],sitefpga) ) {
        cutsize = cutsize + 3 ;
      } // else if
      else {
      } // else
    } // for
    return cutsize ;
  } // CauculateCutsize()
  void selection_sortQ(vector<nodeCandidate> &array, int size) {  // small -> big
    for (int i=0; i<size-1; i++) {
        int min_idx = i;
        for (int j=i+1; j<size; j++) {
            if (array[j].numofCandidate < array[min_idx].numofCandidate) {
                min_idx = j;
            }
        }
        // swap
        nodeCandidate temp = array[min_idx];
        array[min_idx] = array[i];
        array[i] = temp;
    }
  } // selection_sortQ()

  void selection_sortR( NodeFpcut &array, int size) {  // small -> big
    for (int i=0; i<size-1; i++) {
        int min_idx = i;
        for (int j=i+1; j<size; j++) {
            if (array.cutfp[j].cutsize < array.cutfp[min_idx].cutsize) {
                min_idx = j;
            }

        }
        // swap
        int temp1 = array.cutfp[min_idx].cutsize ;
        int temp2 = array.cutfp[min_idx].fpga ;
        array.cutfp[min_idx].cutsize = array.cutfp[i].cutsize ;
        array.cutfp[min_idx].fpga = array.cutfp[i].fpga ;
        array.cutfp[i].cutsize = temp1;
        array.cutfp[i].fpga= temp2 ;
    }
  } // selection_sortR()
  vector<int> vectoperadd(vector<int>a, vector<int>b ) {  // c = a + b
    vector<int> result ;
    int temp = 0 ;
    for( int x = 0 ; x < a.size() ; x++ ) {
       temp = a[x] + b[x] ;
       result.push_back(temp) ;
    } // for
    return result ;
  } // vectoperadd()

   // -----------------------------algorithm2 ---------------------------------------
   // -----------------------------algorithm2 for size big---------------------------------------
  void Bigbigdatapartition( string output ) {
    vector <int> result ;
   	result.resize(GCnode.size(), -1 ) ;
    int putF = 0 ;
    for ( int i = 0 ; i < fixnode.size() ; i++ ) {
      result[fixnode[i]] = GCnode[fixnode[i]].site ; // fixnode?��result don't move Fcapacity is changed already
    } // for
    for ( int j = 0 ; j < GCnode.size() ; j++ ) {
      putF = rand() % GFnode.size() ;
      while ( result[j] == -1 ) {
         if( FCapacity[putF] > 0  ) {
           result[j] = putF;
           FCapacity[putF] -- ;
         } // if
         else
            putF = rand() % GFnode.size() ;
      } // while
    } // for
    Outfile( result, output) ;
  } //
  void Bigdatapartition( string output ) {
    vector <int> result ;
   	result.resize(GCnode.size(), -1 ) ;
   	vector<bigdata> GCnodprioritysmalltohigh ; // GCnodprioritysmalltohigh[0]'s nodenum imply it has smallest neighbor
    bigdata temp ;
    vector<NodeFpcut> priorityR ; // priorityR[i].cutfp[0].fpga  or cutsize  -> node i's 0th fpga and its cut size
    priorityR.resize( GCnode.size() ); // numof C's fpga and its cut size and each node's fpga site will sort in cut size small->big
    Fpcut tempfp ;
    int size = 0 ;

    for ( int i = 0 ; i < GCnode.size() ; i++  ) {
      temp.neighbornum = GCnode[i].neighbors.size() ;
      temp.node = i ;
      GCnodprioritysmalltohigh.push_back(temp) ;
    }// for
    for ( int i = 0 ; i < fixnode.size() ; i++ ) {
      result[fixnode[i]] = GCnode[fixnode[i]].site ; // fixnode?��result don't move Fcapacity is changed already
    } // for

    for ( int j = 0 ; j < Cddt.size() ; j++ ) {   // Add to Q
      for ( int k = 0 ; k < Cddt[j].size() ; k++) { //Cddt[j][k] menas jth node's kth candidate
         tempfp.fpga = Cddt[j][k] ;
         tempfp.cutsize = CauculateCutsize( Cddt[j][k], j, result ) ;
         priorityR[j].cutfp.push_back( tempfp ); //
      } // for
    } // for
    sortGCnode( GCnodprioritysmalltohigh, GCnodprioritysmalltohigh.size() ) ;
    size = GCnodprioritysmalltohigh.size() ;
    for ( int j = 0 ; j < GCnodprioritysmalltohigh.size() ; j++  ) {
      if (  result[GCnodprioritysmalltohigh[j].node] == -1 ) { // doesn't put in
        /*for ( int x = 0 ; x < priorityR[GCnodprioritysmalltohigh[j].node].cutfp.size() ; x++ ) {
          priorityR[GCnodprioritysmalltohigh[j].node].cutfp[x].cutsize = CauculateCutsize( priorityR[GCnodprioritysmalltohigh[j].node].cutfp[x].fpga, GCnodprioritysmalltohigh[j].node, result ) ;
        } // for
        selection_sortR( priorityR[GCnodprioritysmalltohigh[j].node], priorityR[GCnodprioritysmalltohigh[j].node].cutfp.size() );
       if ( FCapacity[priorityR[GCnodprioritysmalltohigh[j].node].cutfp[0].fpga] > 0 ) { // this fpga could put
          result[GCnodprioritysmalltohigh[j].node] = priorityR[GCnodprioritysmalltohigh[j].node].cutfp[0].fpga ;
       } // if
       else { // this Fpga is full
         for ( int x = 1 ; x <  priorityR[GCnodprioritysmalltohigh[j].node].cutfp.size(); x++  ) { // find next could put in fpga
           if ( FCapacity[priorityR[GCnodprioritysmalltohigh[j].node].cutfp[x].fpga] > 0  )  // this fpga could put
             result[GCnodprioritysmalltohigh[j].node]= priorityR[GCnodprioritysmalltohigh[j].node].cutfp[x].fpga ;
         } // for
       } // else
       FCapacity[result[GCnodprioritysmalltohigh[j].node]] -- ; */
        for ( int k = 0 ; k < FCapacity.size() ; k++ ) {
          if ( FCapacity[k]> 0 ) {
            result[GCnodprioritysmalltohigh[j].node] = k;
            FCapacity[k] -- ;
            break ;
          } // if
        } // for
        for ( int x = 0 ; x < GCnode[GCnodprioritysmalltohigh[j].node].neighbors.size(); x++  ) { // put neighbors
           if ( result[GCnode[GCnodprioritysmalltohigh[j].node].neighbors[x]] == -1 ) {
             if ( FCapacity[ result[GCnodprioritysmalltohigh[j].node]] > 0 ) {
               result[GCnode[GCnodprioritysmalltohigh[j].node].neighbors[x] ] = result[GCnodprioritysmalltohigh[j].node] ;
               FCapacity[result[GCnodprioritysmalltohigh[j].node]]-- ;
             } // if
             else {
               for( int h = 0 ; h < GFnode[result[GCnodprioritysmalltohigh[j].node] ].neighbors.size();h++ ) {
                 if ( FCapacity[ GFnode[result[GCnodprioritysmalltohigh[j].node]].neighbors[h]]> 0 ) {
                   result[GCnode[GCnodprioritysmalltohigh[j].node].neighbors[x] ] =  GFnode[result[GCnodprioritysmalltohigh[j].node]].neighbors[h];
                   FCapacity[GFnode[result[GCnodprioritysmalltohigh[j].node]].neighbors[h]]-- ;
                 } // if
               } // for
             } // else put in this node 's fpga's neighbor
          } // if
        }  // for
      } // if

    } // for
    for ( int i = 0 ; i < result.size() ; i ++ ) {
      if ( result[i] == -1 ) {
       for ( int j = 0 ; j < FCapacity.size() ; j ++ ) {
          if ( FCapacity[j] > 0 ) {
            FCapacity[j] -- ;
            result[i] = j ;
          } //if
	    } // for
      } // if
	} // for
    Outfile( result, output) ;
   }  //  Bigdatapartition()

  void sortGCnode(  vector<bigdata> &GCnodprioritysmalltohigh, int size  ) { // max net
    bigdata temp ;
    for (int i=0; i<size-1; i++) {
        int min_idx = i;
        for (int j=i+1; j<size; j++) {
            if (GCnodprioritysmalltohigh[j].neighbornum < GCnodprioritysmalltohigh[min_idx].neighbornum) {
                min_idx = j;
            }
        }
        // swap
        temp.neighbornum = GCnodprioritysmalltohigh[min_idx].neighbornum;
        temp.node = GCnodprioritysmalltohigh[min_idx].node;
        GCnodprioritysmalltohigh[min_idx].neighbornum = GCnodprioritysmalltohigh[i].neighbornum;
        GCnodprioritysmalltohigh[min_idx].node = GCnodprioritysmalltohigh[i].node;
        GCnodprioritysmalltohigh[i].neighbornum = temp.neighbornum;
        GCnodprioritysmalltohigh[i].node = temp.node;

    }
  }


  // -----------------------------algorithm2 for size smaill---------------------------------------
void Outfile( vector<int> result,string outfilename ) {
	FILE *fptr;
	fptr = fopen( outfilename.c_str(), "w") ;
	for ( int i = 0 ; i < result.size(); i++ ) {
		fprintf( fptr , "%d %d\n", i, result[i] ) ;
	} // for
    fclose(fptr);
}

};

int main(int argc, char *argv[]) {
  string inputfile = argv[1] ;
  string outputfile = argv[2] ;
  FILE *fp = fopen( inputfile.c_str(),"r") ;
  char changeline ;
  Input input ;
  vector<int> result ;
  if ( fp == NULL ) {
    printf( "Cannot open file, press any key to exit!\n" ) ;
  }
  fscanf( fp, "%d%d%d%d%d%d%c", &input.TotalFpga, &input.TotalFchannel, &input.CapaciF, &input.TotalNodes, &input.Totalnets,&input.TotalFixnodes,&changeline  ) ;
  FpgaGrah graph( input, fp  ) ;
  if ( input.TotalNodes <= 4000) {
    graph.SetCandidate() ;
    graph.InitialPartion( argv[2] );
  } // if
  else if ( input.TotalNodes <= 10000 ) {
    graph.Bigdatapartition( argv[2] );
  } // else if
  else {
    graph.Bigbigdatapartition(argv[2]) ;
  } // else
  fclose(fp) ;
}
