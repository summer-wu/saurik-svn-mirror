package jMonitor;

public class EventContext {
    private String event_;
    private Class[] types_;
    private Object[] args_;

    public EventContext(String event, Class[] types, Object[] args) {
        event_ = event;
        types_ = types;
        args_ = args;
    }

    public String getEvent() {
        return event_;
    }

    public Class[] getTypes() {
        return types_;
    }

    public Object[] getArguments() {
        return args_;
    }
}
