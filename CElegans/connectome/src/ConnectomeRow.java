package bionet;

import java.util.*;

public class ConnectomeRow
{
   public String  origin;
   public String  target;
   public String  synapse;
   public int     connections;
   public String  transmitter;
   public boolean sensory;
   public boolean motor;

   public ConnectomeRow(String origin, String target, String synapse,
                        int connections, String transmitter)
   {
      this.origin      = origin;
      this.target      = target;
      this.synapse     = synapse;
      this.connections = connections;
      this.transmitter = transmitter;
      sensory          = motor = false;
   }
};
