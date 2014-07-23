// NEURON hoc file document.

#ifndef __NEURON_HOC_DOC_HPP__
#define __NEURON_HOC_DOC_HPP__

#include <string>
#include <vector>
#include <map>
using namespace std;

class NeuronHocDoc
{
public:

   // Synapse.
   class Synapse
   {
public:

      enum TYPE { CHEMICAL, ELECTRICAL } type;
      vector<pair<int, int> >            values;

      Synapse(TYPE type, vector<pair<int, int> > values)
      {
         this->type   = type;
         this->values = values;
      }
   };

   // Document text.
   vector<vector<string> > text;

   // File info.
   pair<int, int> simReference;
   pair<int, int> simsDir;

   // Neurons.
   vector<string> neurons;

   // Synapses:
   // Key: source/target neuron.
   // Value: connection list.
   map<pair<string, string>, vector<Synapse *> > synapses;

   // Stimuli:
   // Key: neuron.
   // Value: delay/duration pair.
   map<string, pair<pair<int, int>, pair<int, int> > > stimuli;

   // Constructor.
   NeuronHocDoc(char *hocFile);

   // Destructor.
   ~NeuronHocDoc();

   // Access synapse.
   Synapse *getSynapse(string source, string target, int connection);

   // Write file.
   bool writeFile(char *filename);

private:

   // Parse file.
   bool parseFile(char *filename);
};
#endif
