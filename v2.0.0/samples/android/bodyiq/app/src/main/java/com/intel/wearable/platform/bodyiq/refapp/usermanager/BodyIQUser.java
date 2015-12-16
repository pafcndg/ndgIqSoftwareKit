package com.intel.wearable.platform.bodyiq.refapp.usermanager;



import java.util.UUID;

/**
 * Created by fissaX on 11/24/15.
 */
public class BodyIQUser {

    UUID    uuid = null;
    String  userId = "";
    String  name = "";
    String  email = "";
    String  password = "";
    float   height = 0.0f;
    float   weight = 0.0f;
    String  deviceName = "";
    String  deviceAddress = "";
    int     gender = 0;
    String  phone = "";

    public static final int MALE    = 0;
    public static final int FEMALE  = 1;
    public static final int UNKNOWN  = 2;

    public UUID getUuid() {
        return uuid;
    }

    public void setUuid(UUID uuid) {
        this.uuid = uuid;
    }

    public String getUserId() {
        return userId;
    }

    public void setUserId(String userId) {
        this.userId = userId;
    }



    public String getName() {
        return name;
    }

    public void setName(String lastName) {
        this.name = lastName;
    }

    public String getEmail() {
        return email;
    }

    public void setEmail(String email) {
        this.email = email;
    }

    public String getPassword() {
        return password;
    }

    public void setPassword(String password) {
        this.password = password;
    }

    public float getHeight() {
        return height;
    }

    public void setHeight(float height) {
        this.height = height;
    }

    public float getWeight() {
        return weight;
    }

    public void setWeight(float weight) {
        this.weight = weight;
    }

    public int getGender() {
        return gender;
    }

    public void setGender(int gender) {
        this.gender = gender;
    }
    public String getPhone() {
        return phone;
    }

    public void setPhone(String phone) {
        this.phone = phone;
    }

    public String getDeviceName() {
        return deviceName;
    }

    public void setDeviceName(String deviceName) {
        this.deviceName = deviceName;
    }

    public String getDeviceAddress() {
        return deviceAddress;
    }

    public void setDeviceAddress(String deviceAddress) {
        this.deviceAddress = deviceAddress;
    }
}
