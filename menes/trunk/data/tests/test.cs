/* Minimal file produces some useful, error-free code. */
public class prog
{
  public extern prog ();

  public static void Main ()
    {
      string[] dests = {
	"rmsg://bfox@hotturd.roamingmessenger.com/hotdope",
	"rmsg://mchuises@developer.roamingmessenger.com/workstation"
      };

      Roamer r = new Roamer (dests, 5);

      RM.DUDE();

      while (r.roam ())
	{
            RM.ALERT(new object[] { 1, 2, 3});
	  RM.MESG ("Thank Goodness!", 10);
	}
    }
}

public class Roamer
{
  public extern Roamer ();

  public int maxLoops;
  public const int MAX_LOOPS_REACHED = 65535;
  private string[] deviceList;
  private int deviceIndex;
  private int deviceLen;
  private int loopCounter;

  public Roamer (string[] destinations, int maxLoops)
    {
      deviceList = destinations;
      deviceIndex = 0;
      deviceLen = deviceList.Length;
      loopCounter = 0;
      this.maxLoops = maxLoops;
    }

  /* Roam to the next device.  Returns TRUE if we are on the next device,
     or FALSE if we exhausted the list. */
  public bool nextDevice ()
    {
      deviceIndex++;
      return (roam ());
    }
      
  /* Roam to the current device, or the next device.
     Returns TRUE if we roam successfully, otherwise,
     FALSE if we exhausted the list. */
  public bool roam ()
    {
      if (loopCounter < maxLoops)
	{
	  if (deviceIndex >= deviceLen)
	    {
	      deviceIndex = 0;

	      loopCounter++;

	      if (loopCounter >= maxLoops)
		return (false);
	    }

	  if (RM.WARP (deviceList[deviceIndex]))
	    return (true);


	  deviceIndex++;
	  return roam ();
	}
      else
	return (false);
    }
}

public class ByteCodeAttribute:System.Attribute
{
    private string assembly_;
    public extern ByteCodeAttribute (string assembly);
}

public class RM
{
  public extern RM ();

  [ByteCode("WARP")]
    public static extern bool WARP (string host);

  [ByteCode("MESG")]
    public static extern Object MESG (Object msg, int timer);

  [ByteCode("ESEL\\n IMPLD 2")]
    public static extern Object ESEL (Object msg, int timer);

  [ByteCode("ALERT")]
    public static extern int ALERT (object[] ns);

  [ByteCode("RSLV")]
    public static extern string[] RSLV (string identifier);

  [ByteCode("PUT")]
    public static extern void PUT (string item);

  [ByteCode("12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890")]
  public static extern void DUDE();
}


