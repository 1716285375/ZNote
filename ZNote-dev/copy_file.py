import os
import shutil

def copy_file(source, destination):
    try:
        # Ensure the destination directory exists
        os.makedirs(destination, exist_ok=True)
        
        # Construct the full path of the destination file
        destination_file = os.path.join(destination, os.path.basename(source))
        
        # If the destination file already exists, remove it
        if os.path.exists(destination_file):
            os.remove(destination_file)
        
        # Copy the file
        shutil.copy(source, destination)
        print(f"The file has been successfully copied to {destination_file}")
    except Exception as e:
        print(f"An error occurred while copying the file: {e}")

if __name__ == "__main__":
    # The mainwindow.ui file in the current directory
    source_file = r"C:\C\Qt\Ui\mainwindow.ui"
    # The target directory
    destination_dir = r"C:\C\Qt\ZNote-dev\ZNote-dev"
    
    # Check if the source file exists
    if os.path.exists(source_file):
        copy_file(source_file, destination_dir)
    else:
        print(f"The source file {source_file} does not exist")