// Create C Elegans bionet: CElegans_network.txt

package bionet;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.*;

public class CElegansBionet
{
   // Usage.
   public static final String Usage =
      "Usage:\n" +
      "    java CElegansBionet\n" +
      "     [-connectomeFileName <path to Excel connectome file>\n" +
      "        (defaults to ../data/CElegansNeuronTables.xls)]\n" +
      "     [-minSynapseWeight <minimum synapse weight (defaults to 0.0)>]\n" +
      "     [-maxSynapseWeight <maximum synapse weight (defaults to 1.0)>]\n" +
      "     [-saveNetwork <network file name(defaults to CElegans_network.txt)>]\n" +
      "     [-randomSeed <random number seed>]";

   public static void main(String[] args)
   {
      String connectomePath   = "../data/CElegansNeuronTables.xls";
      float  minSynapseWeight = 0.0f;
      float  maxSynapseWeight = 1.0f;
      String networkSaveFile  = "CElegans_network.txt";
      int    randomSeed       = 4517;

      for (int i = 0; i < args.length; i++)
      {
         if (args[i].equals("-connectomeFileName"))
         {
            i++;
            if (i >= args.length)
            {
               System.err.println("Invalid connectomeFileName option");
               System.err.println(Usage);
               System.exit(1);
            }
            connectomePath = args[i];
            continue;
         }
         if (args[i].equals("-minSynapseWeight"))
         {
            i++;
            if (i >= args.length)
            {
               System.err.println("Invalid minSynapseWeight option");
               System.err.println(Usage);
               System.exit(1);
            }
            try
            {
               minSynapseWeight = Float.parseFloat(args[i]);
            }
            catch (NumberFormatException e) {
               System.err.println("Invalid minSynapseWeight option");
               System.err.println(Usage);
               System.exit(1);
            }
            if ((minSynapseWeight < 0.0f) || (minSynapseWeight > 1.0f))
            {
               System.err.println("Invalid minSynapseWeight option");
               System.err.println(Usage);
               System.exit(1);
            }
            continue;
         }
         if (args[i].equals("-maxSynapseWeight"))
         {
            i++;
            if (i >= args.length)
            {
               System.err.println("Invalid maxSynapseWeight option");
               System.err.println(Usage);
               System.exit(1);
            }
            try
            {
               maxSynapseWeight = Float.parseFloat(args[i]);
            }
            catch (NumberFormatException e) {
               System.err.println("Invalid maxSynapseWeight option");
               System.err.println(Usage);
               System.exit(1);
            }
            if ((maxSynapseWeight < 0.0f) || (maxSynapseWeight > 1.0f))
            {
               System.err.println("Invalid maxSynapseWeight option");
               System.err.println(Usage);
               System.exit(1);
            }
            continue;
         }
         if (args[i].equals("-saveNetwork"))
         {
            i++;
            if (i >= args.length)
            {
               System.err.println("Invalid saveNetwork option");
               System.err.println(Usage);
               System.exit(1);
            }
            networkSaveFile = args[i];
            continue;
         }
         if (args[i].equals("-randomSeed"))
         {
            i++;
            if (i >= args.length)
            {
               System.err.println("Invalid randomSeed option");
               System.err.println(Usage);
               System.exit(1);
            }
            try
            {
               randomSeed = Integer.parseInt(args[i]);
            }
            catch (NumberFormatException e) {
               System.err.println("Invalid randomSeed option");
               System.err.println(Usage);
               System.exit(1);
            }
            continue;
         }
         System.err.println(Usage);
         System.exit(1);
      }
      if (minSynapseWeight > maxSynapseWeight)
      {
         System.err.println("minSynapseWeight > maxSynapseWeight");
         System.err.println(Usage);
         System.exit(1);
      }

      ConnectomeTables connectomeTables = new ConnectomeTables(connectomePath);
      try
      {
         connectomeTables.read();
      }
      catch (IOException e) {
         e.printStackTrace();
      }
      save(connectomeTables, minSynapseWeight, maxSynapseWeight,
           networkSaveFile, randomSeed);
   }


   // Save network.
   public static void save(ConnectomeTables connectomeTables,
                           float minSynapseWeight, float maxSynapseWeight,
                           String networkSaveFile, int randomSeed)
   {
      int numNeurons, numSensors, numMotors;

      // Count neurons.
      numNeurons = numSensors = numMotors = 0;
      HashSet<String> neuronTypes = new HashSet<String>();
      for (int i = 0; i < connectomeTables.connectome.size(); i++)
      {
         ConnectomeRow neuron = connectomeTables.connectome.get(i);
         if (!neuronTypes.contains(neuron.origin))
         {
            neuronTypes.add(neuron.origin);
            numNeurons++;
            if (neuron.sensory)
            {
               numSensors++;
            }
         }
         if (neuron.motor)
         {
            if (!neuronTypes.contains(neuron.target))
            {
               neuronTypes.add(neuron.target);
               numNeurons++;
               numMotors++;
            }
         }
      }

      // Assign indices and excitatory state.
      HashMap<String, Integer> neuronIndices = new HashMap<String, Integer>();
      String[] neuronNames   = new String[numNeurons];
      int[] neuronExcitatory = new int[numNeurons];
      int s, m, n;
      s = 0;
      m = numSensors;
      n = numSensors + numMotors;
      for (int i = 0; i < connectomeTables.connectome.size(); i++)
      {
         ConnectomeRow neuron = connectomeTables.connectome.get(i);
         if (!neuronIndices.containsKey(neuron.origin))
         {
            if (neuron.sensory)
            {
               neuronIndices.put(neuron.origin, new Integer(s));
               neuronNames[s] = neuron.origin;
               if (neuron.transmitter.startsWith("GABA"))
               {
                  neuronExcitatory[s] = 0;
               }
               else
               {
                  neuronExcitatory[s] = 1;
               }
               s++;
            }
            else
            {
               neuronIndices.put(neuron.origin, new Integer(n));
               neuronNames[n] = neuron.origin;
               if (neuron.synapse.startsWith("GABA"))
               {
                  neuronExcitatory[n] = 0;
               }
               else
               {
                  neuronExcitatory[n] = 1;
               }
               n++;
            }
         }
         if (neuron.motor)
         {
            if (!neuronIndices.containsKey(neuron.target))
            {
               neuronIndices.put(neuron.target, new Integer(m));
               neuronNames[m]      = neuron.target;
               neuronExcitatory[m] = 1;
               m++;
            }
         }
      }

      // Configure synapses.
      String[][] synapses = new String[numNeurons][numNeurons];
      for (int i = 0; i < numNeurons; i++)
      {
         for (int j = 0; j < numNeurons; j++)
         {
            synapses[i][j] = null;
         }
      }
      for (int i = 0; i < connectomeTables.connectome.size(); i++)
      {
         ConnectomeRow neuron = connectomeTables.connectome.get(i);
         m = neuronIndices.get(neuron.origin).intValue();
         n = neuronIndices.get(neuron.target).intValue();
         synapses[m][n] = neuron.transmitter;
      }

      try
      {
         File file = new File(networkSaveFile);

         if (!file.exists())
         {
            file.createNewFile();
         }

         FileWriter     fw = new FileWriter(file.getAbsoluteFile());
         BufferedWriter bw = new BufferedWriter(fw);
         bw.write(numNeurons + "\n");
         bw.write(numSensors + "\n");
         bw.write(numMotors + "\n");
         for (int i = 0; i < numNeurons; i++)
         {
            bw.write(i + "\n");
            bw.write(neuronExcitatory[i] + "\n");
            bw.write(1 + "\n");
            bw.write(0.0f + "\n");
            bw.write(0.0f + "\n");
            bw.write("\"" + neuronNames[i] + "\"\n");
         }
         n = 0;
         for (int i = 0; i < numNeurons; i++)
         {
            for (int j = 0; j < numNeurons; j++)
            {
               if (synapses[i][j] != null)
               {
                  n++;
               }
            }
         }
         bw.write(n + "\n");
         Random random = new Random(randomSeed);
         float  scale  = maxSynapseWeight - minSynapseWeight;
         for (int i = 0; i < numNeurons; i++)
         {
            for (int j = 0; j < numNeurons; j++)
            {
               if (synapses[i][j] != null)
               {
                  bw.write(i + "\n");
                  bw.write(j + "\n");
                  bw.write(1 + "\n");
                  bw.write(((random.nextFloat() * scale) + minSynapseWeight) + "\n");
                  bw.write(0 + "\n");
                  bw.write(0.0f + "\n");
                  bw.write("\"" + synapses[i][j] + "\"\n");
               }
            }
         }
         bw.close();
      }
      catch (IOException e) {
         e.printStackTrace();
      }
   }
}
