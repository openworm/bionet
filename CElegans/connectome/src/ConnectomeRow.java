package bionet;

import java.util.*;

public class ConnectomeRow
{
   public String  origin;
   public String  target;
   public String  type;
   public int     connections;
   public String  transmitter;
   public boolean sensory;
   public boolean motor;

   public ConnectomeRow(String origin, String target, String type,
                        int connections, String transmitter)
   {
      this.origin      = origin;
      this.target      = target;
      this.type        = type;
      this.connections = connections;
      this.transmitter = transmitter;
      sensory          = motor = false;
   }
};
