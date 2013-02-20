/** Contains a coordinate pair. 
 * Generaly it is used to label a site on a lattice.
*/
#define INF 1000.0
#define V 0.31831 // =1/PI
#define P4R_MAX2 3600.0 // =4*r^{2}_max

typedef struct SITIO sitio;
typedef struct AGENT agent;
typedef struct AGENTLIST agentList;
typedef struct EVENT event;
typedef struct NODE node;
typedef struct DATA_TREE data_tree;

struct SITIO{
float i;			/**< i coordinate, could be thinked as the X coordinate*/ 
float j;			/**< j coordinate, could be thinked as the Y coordinate*/ 
};

struct AGENTLIST{
agent **members;
int NoMembers;
};

struct AGENT{			
sitio position;
float radius;	
float time;
int GrowthRate;
agentList vecinos;
event *NextInteraction;	
};

struct EVENT{
float time;
agent *indv1;
agent *indv2;
node *Node;
};

struct NODE{
event *data;
node *before;
node *after;
node *parent;
};

struct DATA_TREE{
node *root;
node *first;
};

void init();

//// Tree list managment
node* newNode(event *new);

node* Dettach_Node(node *to_detach,node *root);

node* Attacht_Node(node *to_attacht,node *root);

node* Min_Element(node *root);


//// Manage event
event* newAppendEvent(agent *Agent1,agent *Agent2, float t);

void Insert_Event_In_Queue(event *new, data_tree *queue);

void Process_Event(event *collision);

void Update_Next_Event(agent *indv);

float Process_Queue(data_tree *queue);

/////////////////// manage agent
agent* newAgent(sitio pos,float r, float t, int GrowthRate);

void InsertAgentInField(agent *AgentToInsert,agentList *field);

void newAppendAgentToList(agent *Agent, agentList *list);

void FastForward(agentList list,float Time);

///////Auxiliaries
void WriteStateToFile(agentList list,char *fileName,float Time);

void FreeSystem(agentList *Field);

//////Grapers de modelo
float event_time(const agent *indv1,const agent *indv2);

//////Modelo y parametros de crecimiento:
float Radius(float r1, float t1,float t2);

float ContactTime(float r11,float t11,float r12,float t12,float d2);

float ContacTime1(float r1,float t1,float d);

