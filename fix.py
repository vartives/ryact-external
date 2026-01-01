import os
import sys

def add_pragma_once_to_headers(directory):
    """
    Durchsucht rekursiv ein Verzeichnis nach .h Dateien und fügt
    #pragma once in der ersten Zeile hinzu, falls nicht vorhanden.
    """
    files_modified = 0
    files_skipped = 0
    files_error = 0
    
    print(f"Durchsuche Verzeichnis: {directory}\n")
    
    # Rekursiv durch alle Dateien und Unterordner gehen
    for root, dirs, files in os.walk(directory):
        for filename in files:
            # Nur .h Dateien verarbeiten
            if filename.endswith('.h'):
                filepath = os.path.join(root, filename)
                
                try:
                    # Datei lesen
                    with open(filepath, 'r', encoding='utf-8') as f:
                        content = f.read()
                    
                    # Prüfen ob #pragma once bereits vorhanden ist
                    if '#pragma once' in content:
                        print(f"✓ Übersprungen (bereits vorhanden): {filepath}")
                        files_skipped += 1
                    else:
                        # #pragma once am Anfang hinzufügen
                        new_content = '#pragma once\n\n' + content
                        
                        # Datei mit neuem Inhalt schreiben
                        with open(filepath, 'w', encoding='utf-8') as f:
                            f.write(new_content)
                        
                        print(f"✓ Geändert: {filepath}")
                        files_modified += 1
                        
                except Exception as e:
                    print(f"✗ Fehler bei {filepath}: {str(e)}")
                    files_error += 1
    
    # Zusammenfassung
    print("\n" + "="*60)
    print("ZUSAMMENFASSUNG:")
    print(f"  Dateien geändert:     {files_modified}")
    print(f"  Dateien übersprungen: {files_skipped}")
    print(f"  Fehler:               {files_error}")
    print("="*60)

if __name__ == "__main__":
    # Verzeichnis aus Kommandozeilenargument oder aktuelles Verzeichnis
    if len(sys.argv) > 1:
        target_directory = sys.argv[1]
    else:
        target_directory = "."
    
    # Prüfen ob Verzeichnis existiert
    if not os.path.isdir(target_directory):
        print(f"Fehler: '{target_directory}' ist kein gültiges Verzeichnis!")
        sys.exit(1)
    
    # Sicherheitsabfrage
    print(f"Dieses Skript wird alle .h Dateien in '{target_directory}' (rekursiv) bearbeiten.")
    response = input("Fortfahren? (j/n): ")
    
    if response.lower() in ['j', 'ja', 'y', 'yes']:
        add_pragma_once_to_headers(target_directory)
    else:
        print("Abgebrochen.")