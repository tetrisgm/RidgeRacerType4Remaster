"""Linux release compatibility boundaries must fail closed."""
import sys
from pathlib import Path
import unittest
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from stage_linux import audit_elf

GOOD = """Class: ELF64
Machine: Advanced Micro Devices X86-64
Shared library: [libc.so.6]
Shared library: [libGL.so.1]
Name: GLIBC_2.2.5
Name: GLIBC_2.31
"""

class LinuxAuditTests(unittest.TestCase):
    def test_supported_baseline(self):
        self.assertEqual(audit_elf(GOOD), (['libc.so.6', 'libGL.so.1'], '2.31'))

    def test_newer_glibc_is_not_portable(self):
        with self.assertRaises(ValueError):
            audit_elf(GOOD + 'Name: GLIBC_2.34\n')

    def test_unbundled_runtime_is_rejected(self):
        with self.assertRaises(ValueError):
            audit_elf(GOOD + 'Shared library: [libstdc++.so.6]\n')

    def test_wrong_architecture_is_rejected(self):
        with self.assertRaises(ValueError):
            audit_elf(GOOD.replace('Advanced Micro Devices X86-64', 'AArch64'))

if __name__ == '__main__':
    unittest.main()
