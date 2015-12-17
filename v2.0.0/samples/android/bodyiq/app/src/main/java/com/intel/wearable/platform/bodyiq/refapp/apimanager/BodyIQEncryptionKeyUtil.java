package com.intel.wearable.platform.bodyiq.refapp.apimanager;

import android.content.Context;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.security.SecureRandom;

/**
 * Created by fissaX on 12/11/15.
 */
public class BodyIQEncryptionKeyUtil {

    private static byte[] mKey = null;

    private static final String KEY_FILE = "KEY";

    public synchronized static byte[] getKey(Context context) {
        if (mKey == null) {
            File installation = new File(context.getFilesDir(), KEY_FILE);
            try {
                if (!installation.exists()) {
                    writeKeyFile(installation);
                }
                mKey = readKeyFile(installation);
            } catch (Exception e) {
                throw new RuntimeException(e);
            }
        }
        return mKey;
    }

    private static byte[] readKeyFile(File installation) throws IOException {
        RandomAccessFile f = new RandomAccessFile(installation, "r");
        byte[] bytes = new byte[(int) f.length()];
        f.readFully(bytes);
        f.close();
        return bytes;
    }

    private static void writeKeyFile(File installation) throws IOException {
        FileOutputStream out = new FileOutputStream(installation);
        final byte[] key = new byte[64];
        (new SecureRandom()).nextBytes(key);
        out.write(key);
        out.close();
    }
}
