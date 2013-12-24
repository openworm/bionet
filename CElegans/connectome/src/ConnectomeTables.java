package bionet;

import java.io.File;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.text.ParseException;
import java.util.*;

import jxl.Cell;
import jxl.CellType;
import jxl.Sheet;
import jxl.Workbook;
import jxl.read.biff.BiffException;

class ConnectomeTables
{
   public ArrayList<ConnectomeRow> connectome;

   private String inputFile;

   public ConnectomeTables(String inputFile)
   {
      this.inputFile = inputFile;
      connectome     = new ArrayList<ConnectomeRow>();
   }


   public void read() throws IOException
   {
      File     inputWorkbook = new File(inputFile);
      Workbook w;

      try {
         w = Workbook.getWorkbook(inputWorkbook);

         for (int s = 0; s < 3; s++)
         {
            Sheet sheet = w.getSheet(s);
            switch (s)
            {
            // Connectome.
            case 0:
               for (int i = 1; i < sheet.getRows(); i++)
               {
                  Cell   cell   = sheet.getCell(0, i);
                  String origin = cell.getContents();
                  cell = sheet.getCell(1, i);
                  String target = cell.getContents();
                  cell = sheet.getCell(2, i);
                  String synapse = cell.getContents();
                  cell = sheet.getCell(3, i);
                  String connections = cell.getContents();
                  cell = sheet.getCell(4, i);
                  String        transmitter = cell.getContents();
                  ConnectomeRow neuron      = new ConnectomeRow(origin, target, synapse,
                                                                Integer.parseInt(connections),
                                                                transmitter);
                  connectome.add(neuron);
               }
               break;

            case 1:
               // Motor neurons.
               for (int i = 1; i < sheet.getRows(); i++)
               {
                  Cell   cell   = sheet.getCell(0, i);
                  String origin = cell.getContents();
                  cell = sheet.getCell(1, i);
                  String target = cell.getContents();
                  cell = sheet.getCell(2, i);
                  String connections = cell.getContents();
                  cell = sheet.getCell(3, i);
                  String        transmitter = cell.getContents();
                  ConnectomeRow neuron      = new ConnectomeRow(origin, target, "Muscle",
                                                                Integer.parseInt(connections),
                                                                transmitter);
                  neuron.motor = true;
                  connectome.add(neuron);
               }
               break;

            case 2:
               // Sensory neurons.
               for (int i = 1; i < sheet.getRows(); i++)
               {
                  Cell   cell   = sheet.getCell(0, i);
                  String origin = cell.getContents();
                  for (int j = 0; j < connectome.size(); j++)
                  {
                     ConnectomeRow neuron = connectome.get(j);
                     if (neuron.origin.equals(origin))
                     {
                        neuron.sensory = true;
                     }
                  }
               }
               break;
            }
         }
      }
      catch (BiffException e) {
         e.printStackTrace();
      }
   }


   public void print()
   {
      System.out.println("Connectome:");
      System.out.println("origin\ttarget\tsynapse\tconnections\ttransmitter\tsensory\tmotor");
      for (int i = 0; i < connectome.size(); i++)
      {
         ConnectomeRow neuron  = connectome.get(i);
         String        sensory = "false";
         if (neuron.sensory) { sensory = "true"; }
         String motor = "false";
         if (neuron.motor) { motor = "true"; }
         System.out.println(neuron.origin + "\t" + neuron.target + "\t" +
                            neuron.synapse + "\t" + neuron.connections + "\t" +
                            neuron.transmitter + "\t" + sensory + "\t" + motor);
      }
   }
}
