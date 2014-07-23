// NEURON hoc file document.

#include "neuronHocDoc.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Constructor.
NeuronHocDoc::NeuronHocDoc(char *hocFile)
{
   parseFile(hocFile);
}


// Destructor.
NeuronHocDoc::~NeuronHocDoc()
{
   int i, j;

   map<pair<string, string>, vector<Synapse *> >::iterator itr;

   for (itr = synapses.begin(); itr != synapses.end(); ++itr)
   {
      for (i = 0, j = (int)itr->second.size(); i < j; i++)
      {
         delete itr->second[i];
      }
      itr->second.clear();
   }
}


// Access synapse.
NeuronHocDoc::Synapse *NeuronHocDoc::getSynapse(string source, string target, int connection)
{
   map<pair<string, string>, vector<Synapse *> >::iterator itr;

   itr = synapses.find(pair<string, string>(source, target));
   if (itr != synapses.end())
   {
      if ((connection >= 0) && (connection < (int)itr->second.size()))
      {
         return(itr->second[connection]);
      }
   }
   return(NULL);
}


// Write file.
bool NeuronHocDoc::writeFile(char *filename)
{
   int i, j, p, q;

   FILE *fp;

   if ((fp = fopen(filename, "wb")) == NULL)
   {
      return(false);
   }
   for (i = 0, j = (int)text.size(); i < j; i++)
   {
      for (p = 0, q = (int)text[i].size(); p < q; p++)
      {
         fprintf(fp, "%s", text[i][p].c_str());
      }
      fprintf(fp, "\n");
   }
   fclose(fp);
   return(true);
}


/*
 * Parse file.
 *
 * Synapse formats:
 * Chemical:
 * new NetCon(&v(0.46315283), syn_NCXLS_I4_I2R_Glutamate[4], 0.0, 0.0, 1.0))}
 * Electrical:
 * elecsyn_NCXLS_M3R_MCR_GJ_Generic_GJ_A[0].weight = 1.0
 * elecsyn_NCXLS_M3R_MCR_GJ_Generic_GJ_B[0].weight = 1.0
 */
bool NeuronHocDoc::parseFile(char *filename)
{
   ifstream inFile;
   string   strLine;

   vector<string>          line;
   int                     simRefLen, simsDirLen;
   int                     chemSynLen, elecSynLen;
   int                     delLen, durLen;
   string                  prefix, source, target, delay, weight;
   pair<int, int>          firstWeight, delVal, durVal;
   vector<pair<int, int> > values;
   Synapse                 *synapse;
   vector<Synapse *>       connections;
   map<pair<string, string>, vector<Synapse *> >::iterator itr;
   char buf[BUFSIZ + 1], c, *s1, *s2, *s3, *s4, *s5, *s6, *s7;
   int  i, j;

   inFile.open(filename);
   if (!inFile.is_open())
   {
      return(false);
   }

   simRefLen         = strlen("simReference = \"");
   simsDirLen        = strlen("simsDir = \"");
   chemSynLen        = strlen("syn_NCXLS_");
   elecSynLen        = strlen("elecsyn_NCXLS_");
   delLen            = strlen(".del = ");
   durLen            = strlen(".dur = ");
   firstWeight.first = -1;
   while (inFile)
   {
      getline(inFile, strLine);
      memset(buf, 0, BUFSIZ + 1);
      strncpy(buf, strLine.c_str(), BUFSIZ);
      line.clear();

      // Parse simulation reference and directory.
      if ((s1 = strstr(buf, "simReference = \"")) != NULL)
      {
         s2 = s1 + simRefLen;
         if (*s2 != '\0')
         {
            c   = *s2;
            *s2 = '\0';
            line.push_back(buf);
            *s2 = c;
            if ((s3 = strstr(s2, "\"")) != NULL)
            {
               *s3 = '\0';
               line.push_back(s2);
               *s3 = '"';
               line.push_back(s3);
               text.push_back(line);
               simReference = pair<int, int>(text.size() - 1, 1);
               continue;
            }
         }
      }
      if ((s1 = strstr(buf, "simsDir = \"")) != NULL)
      {
         s2 = s1 + simsDirLen;
         if (*s2 != '\0')
         {
            c   = *s2;
            *s2 = '\0';
            line.push_back(buf);
            *s2 = c;
            if ((s3 = strstr(s2, "\"")) != NULL)
            {
               *s3 = '\0';
               line.push_back(s2);
               *s3 = '"';
               line.push_back(s3);
               text.push_back(line);
               simsDir = pair<int, int>(text.size() - 1, 1);
               continue;
            }
         }
      }

      // Parse stimulus.
      if ((s1 = strstr(buf, ".del = ")) != NULL)
      {
         s2 = s1 + delLen;
         if (*s2 != '\0')
         {
            c   = *s2;
            *s2 = '\0';
            line.push_back(buf);
            *s2 = c;
            for (s3 = s2; *s3 != ' ' && *s3 != '\n' && *s3 != '\0'; s3++)
            {
            }
            *s3 = '\0';
            line.push_back(s2);
            text.push_back(line);
            delVal = pair<int, int>(text.size() - 1, 1);
            getline(inFile, strLine);
            memset(buf, 0, BUFSIZ + 1);
            strncpy(buf, strLine.c_str(), BUFSIZ);
            line.clear();
            if ((s1 = strstr(buf, ".dur = ")) != NULL)
            {
               s2 = s1 + durLen;
               if (*s2 != '\0')
               {
                  c   = *s2;
                  *s2 = '\0';
                  line.push_back(buf);
                  *s2 = c;
                  for (s3 = s2; *s3 != ' ' && *s3 != '\n' && *s3 != '\0'; s3++)
                  {
                  }
                  *s3 = '\0';
                  line.push_back(s2);
                  text.push_back(line);
                  durVal = pair<int, int>(text.size() - 1, 1);
                  for (s4 = s2; *s4 != '[' && s4 > buf; s4--)
                  {
                  }
                  if (s4 > buf)
                  {
                     *s4 = '\0';
                     for (s5 = s4; *s5 != '_' && s5 > buf; s5--)
                     {
                     }
                     if (s5 > buf)
                     {
                        s5++;
                        source          = s5;
                        stimuli[source] =
                           pair<pair<int, int>, pair<int, int> >(delVal, durVal);
                        continue;
                     }
                  }
               }
            }
         }
      }

      // Parse synapse.
      if (((s1 = strstr(buf, "new NetCon")) != NULL) &&
          ((s2 = strstr(buf, "syn_NCXLS_")) != NULL))
      {
         // Chemical synapse.
         s3 = s2 + chemSynLen;
         if (*s3 != '\0')
         {
            for (s4 = s3; *s4 != '_' && *s4 != '\0'; s4++)
            {
            }
            if (*s4 == '_')
            {
               *s4    = '\0';
               source = s3;
               if (source == "MI")
               {
                  source = "M1";
               }
               *s4 = '_';
               s4++;
               for (s5 = s4; *s5 != '_' && *s5 != '\0'; s5++)
               {
               }
               if (*s5 == '_')
               {
                  *s5    = '\0';
                  target = s4;
                  if (target == "MI")
                  {
                     target = "M1";
                  }
                  *s5 = '_';
                  for ( ; *s5 != ',' && *s5 != '\0'; s5++)
                  {
                  }
                  if (*s5 == ',')
                  {
                     s5++;
                     for ( ; *s5 != ',' && *s5 != '\0'; s5++)
                     {
                     }
                     if (*s5 == ',')
                     {
                        s5++;
                        if (*s5 == ' ')
                        {
                           *s5 = '\0';
                           s5++;
                           if (*s5 != '\0')
                           {
                              for (s6 = s5; *s6 != ',' && *s6 != '\0'; s6++)
                              {
                              }
                              if (*s6 == ',')
                              {
                                 *s6   = '\0';
                                 delay = s5;
                                 s6++;
                                 if (*s6 == ' ')
                                 {
                                    s6++;
                                    if (*s6 != '\0')
                                    {
                                       for (s7 = s6; *s7 != ')' && *s7 != '\0'; s7++)
                                       {
                                       }
                                       if (*s7 == ')')
                                       {
                                          *s7    = '\0';
                                          weight = s6;
                                          prefix = buf;
                                          line.push_back(prefix);
                                          line.push_back(" ");
                                          line.push_back(delay);
                                          line.push_back(", ");
                                          line.push_back(weight);
                                          line.push_back("))}");
                                          text.push_back(line);
                                          values.clear();
                                          values.push_back(pair<int, int>(text.size() - 1, 2));
                                          values.push_back(pair<int, int>(text.size() - 1, 4));
                                          synapse = new Synapse(NeuronHocDoc::Synapse::CHEMICAL, values);
                                          assert(synapse != NULL);
                                          itr = synapses.find(pair<string, string>(source, target));
                                          if (itr == synapses.end())
                                          {
                                             connections.clear();
                                             connections.push_back(synapse);
                                             synapses[pair < string, string > (source, target)] = connections;
                                             for (i = 0, j = (int)neurons.size(); i < j; i++)
                                             {
                                                if (neurons[i] == source)
                                                {
                                                   break;
                                                }
                                             }
                                             if (i == j)
                                             {
                                                neurons.push_back(source);
                                             }
                                             for (i = 0, j = (int)neurons.size(); i < j; i++)
                                             {
                                                if (neurons[i] == target)
                                                {
                                                   break;
                                                }
                                             }
                                             if (i == j)
                                             {
                                                neurons.push_back(target);
                                             }
                                          }
                                          else
                                          {
                                             itr->second.push_back(synapse);
                                          }
                                          continue;
                                       }
                                    }
                                 }
                              }
                           }
                        }
                     }
                  }
               }
            }
         }
      }
      else
      {
         if (((s1 = strstr(buf, "elecsyn_NCXLS_")) != NULL) &&
             ((s2 = strstr(buf, ".weight = ")) != NULL))
         {
            // Electrical synapse.
            s3 = s1 + elecSynLen;
            if (*s3 != '\0')
            {
               for (s4 = s3; *s4 != '_' && *s4 != '\0'; s4++)
               {
               }
               if (*s4 == '_')
               {
                  *s4    = '\0';
                  source = s3;
                  if (source == "MI")
                  {
                     source = "M1";
                  }
                  *s4 = '_';
                  s4++;
                  for (s5 = s4; *s5 != '_' && *s5 != '\0'; s5++)
                  {
                  }
                  if (*s5 == '_')
                  {
                     *s5    = '\0';
                     target = s4;
                     if (target == "MI")
                     {
                        target = "M1";
                     }
                     *s5 = '_';
                     for ( ; *s5 != '=' && *s5 != '\0'; s5++)
                     {
                     }
                     if (*s5 == '=')
                     {
                        s5++;
                        for ( ; *s5 == ' '; s5++)
                        {
                        }
                        if (*s5 != '\0')
                        {
                           for (s6 = s5; *s6 != ' ' && *s6 != '\0'; s6++)
                           {
                           }
                           *s6    = '\0';
                           weight = s5;
                           *s5    = '\0';
                           prefix = buf;
                           line.push_back(prefix);
                           line.push_back(weight);
                           text.push_back(line);
                           if (firstWeight.first == -1)
                           {
                              firstWeight = pair<int, int>(text.size() - 1, 1);
                           }
                           else
                           {
                              values.clear();
                              values.push_back(firstWeight);
                              firstWeight.first = -1;
                              values.push_back(pair<int, int>(text.size() - 1, 1));
                              synapse = new Synapse(NeuronHocDoc::Synapse::ELECTRICAL, values);
                              assert(synapse != NULL);
                              itr = synapses.find(pair<string, string>(source, target));
                              if (itr == synapses.end())
                              {
                                 connections.clear();
                                 connections.push_back(synapse);
                                 synapses[pair < string, string > (source, target)] = connections;
                                 for (i = 0, j = (int)neurons.size(); i < j; i++)
                                 {
                                    if (neurons[i] == source)
                                    {
                                       break;
                                    }
                                 }
                                 if (i == j)
                                 {
                                    neurons.push_back(source);
                                 }
                                 for (i = 0, j = (int)neurons.size(); i < j; i++)
                                 {
                                    if (neurons[i] == target)
                                    {
                                       break;
                                    }
                                 }
                                 if (i == j)
                                 {
                                    neurons.push_back(target);
                                 }
                              }
                              else
                              {
                                 itr->second.push_back(synapse);
                              }
                           }
                           continue;
                        }
                     }
                  }
               }
            }
         }
      }
      line.push_back(strLine);
      text.push_back(line);
   }
   sort(neurons.begin(), neurons.end());
   inFile.close();
   return(true);
}
