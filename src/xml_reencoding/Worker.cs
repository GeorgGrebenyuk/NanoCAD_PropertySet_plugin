using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;
using System.IO;
using System.Reflection;

namespace xml_reencoding
{
    class Worker
    {
        static void Main(string[] args)
        {
            string path_psets_folder = $@"C:\Users\{Environment.UserName}\AppData\Roaming\ncad_psets";
            foreach (string dir_path_row in Directory.GetDirectories(Path.Combine(path_psets_folder, "_temp"), 
                "*.*", SearchOption.TopDirectoryOnly ))
            {
                string new_dir_path = Path.Combine(path_psets_folder, "saves", new DirectoryInfo(dir_path_row).Name);
                if (!Directory.Exists(new_dir_path)) Directory.CreateDirectory(new_dir_path);
                foreach (string file in Directory.GetFiles(dir_path_row, "*.*", SearchOption.AllDirectories))
                {
                    string new_file_path = Path.Combine(new_dir_path, Path.GetFileName(file));

                    StreamReader sr = new StreamReader(file);
                    StreamWriter sw = new StreamWriter(new_file_path, false, Encoding.Default);

                    // invoke the ReadToEnd method
                    sw.WriteLine(sr.ReadToEnd());
                    sw.Close();
                    sr.Close();
                }
                //Directory.Delete(dir_path_row, true);
            }
        }
    }
}
