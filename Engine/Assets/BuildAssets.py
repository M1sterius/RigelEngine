import sys

if __name__ == "__main__":
    build_type = sys.argv[1] if len(sys.argv) > 1 else "Unknown"
    output_path = sys.argv[2] if len(sys.argv) >= 2 else "Unknown"

