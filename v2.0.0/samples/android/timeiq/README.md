# TimeIQ

### Initialization

You will need to initialize the TimeIQApi in your main activity's OnCreate or in the service as seen below.
You can create several instances of TimeIQApi, or use a static member as seen below.
The following is an example of how this can be done:

```java
IAuthCredentialsProvider myAuthProvider = AuthUtil.getAuthProvider(getApplicationContext()); // your authentication provider
Context context = getApplicationContext();
String cloudServerUrl = getCloudServerURL(); // the authentication's server URL
boolean initWasSuccessful = TimeIQBGService.mTimeIQApi.init(context, myAuthProvider, cloudServerUrl); // see the next section about the background service

if (initWasSuccessful) {
	// Success - do stuff...
} else {
	// Failed to initialize - show error \ exit
}
```

### Listen to messages

You should listen to TimeIQ messages, such as:
* A reminder was triggered.
* Time to leave for an event.

It is best practice to do so from within a sticky service - so you will get it even when your application is down.

```java
public class TimeIQBGService extends Service implements IMessageListener {

    private static final String TAG = TimeIQBGService.class.getSimpleName();
    public static final TimeIQApi mTimeIQApi = new TimeIQApi();

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        Context context = getApplicationContext();
        IAuthCredentialsProvider authProvider = getAuthProvider(); // you will need to implement and authentication provider
        authProvider.loadData();
        if (!authProvider.isUserLoggedIn()) {
        // we're running from the background service, make sure we are trying to login before initializing the SDK
        // login here
        }

        // init the timeIq engine and start it's engines:
        String cloudServerUrl = getCloudServerURL(); // the authentication's server URL
        boolean initWasSuccessful = TimeIQBGService.mTimeIQApi.init(context, authProvider, cloudServerUrl);

        if (initWasSuccessful) {
            initTimeIQMessageListener();
        }
    }
    
    private void initTimeIQMessageListener() {
    // Add listener to TimeIQ messages:
        IMessageHandler timeIQMessageHandler = TimeIQBGService.mTimeIQApi.getMessageHandler();
        timeIQMessageHandler.register(this); // register only adds a listener. Messages will be queued until the messageHandler will be initialized.
        timeIQMessageHandler.init(); // start getting messages
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        if (mTimeIQApi.isInitialized()) {
            return Service.START_STICKY; // init was OK --> start sticky
        } else {
        	return Service.START_NOT_STICKY; // there was a problem --> not sticky
        }
    }

    @Override
    public void onDestroy() {
        IMessageHandler timeIQMessageHandler = TimeIQBGService.mTimeIQApi.getMessageHandler();
        timeIQMessageHandler.unRegister(this);
        TimeIQBGService.mTimeIQApi.onDestroy();
        super.onDestroy();
    }

	/**
     * A message received from TimeIQ, the message contains a type, and a data that correspond to that type
     * @param message - The SDK message
     */
    @Override
    public void onReceive(IMessage message) {
        MessageType messageType = (MessageType) message.getType();
        switch (messageType){
            case ON_REMINDERS_TRIGGERED:
                RemindersResult remindersResult = (RemindersResult) message.getData();
                // The data contains a collection of reminders, each of them was triggered:
                Collection<IReminder> remindersCollection = remindersResult.getResult();
                // onRemindersTriggered(remindersCollection); - do something when the reminder is triggered
                break;
            case ON_EVENT_TRIGGERED:
                TSOEventTriggeredMsg eventTriggeredResult = (TSOEventTriggeredMsg) message.getData();
                // onEventTriggered(eventTriggeredResult); - do something when the event is triggered
                break;
            case ON_EVENT_START:
                TSOEventTriggeredMsg eventTriggeredResult = (TSOEventTriggeredMsg) message.getData();
                // onEventTriggered(eventTriggeredResult); - do something when the event is starts
                break;
            case ON_EVENT_END:
                TSOEventTriggeredMsg eventTriggeredResult = (TSOEventTriggeredMsg) message.getData();
                // onEventTriggered(eventTriggeredResult); - do something when the event is ends
                break;
        }
    }
}
```
